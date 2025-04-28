/* "$Id: $"
 *
 *                          10 October MMIX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <cc/Scope.h>

static void __init__(Ex **e__)
{
	(void)e__;
	cc__Scope__.super = &Object__;
	return;
}


static void __ctor__(Ex **e__, cc__Scope *self)
{
	F("__ctor__");

	(void)__func__;
	(void)e__;

	self->parent = 0;
	self->objects = new_(Hash);
	self->current_object = 0;
	self->bracket = 0;
	self->brace = 0;
	self->paranthesis = 0;
	self->last_rett_id = 0;
	return;
}

static void __dtor__(Ex **e__, cc__Scope *self)
{
	F("__dtor__");

	(void)__func__;

	self->objects->_->clear_objects(e__, self->objects);
	CHECK(error1);
error1:
	delete_(self->objects);
	CHECK(error2);
error2:
	return;
}

static uint32 hash_code(Ex **e__, cc__Scope *self)
{
	(void)e__;
#ifdef _MSC_VER
	return *((uint32*)&self) >> 4;
#else 
	return (uint32)self;
#endif

} 

static void to_string(Ex **e__, cc__Scope *self, String *out)
{
	F("to_string");
	String s;

	STR(s, self->_->__name__);
	out->_->add(e__, out, &s);
	CHECK(error);
error:
	return;
}

static cc__Type *get_type_of(Ex **e__, cc__Scope *self, String *name)
{
	F("get_type_of");
	cc__Type *t;
	
	t = self->objects->_->get(e__, self->objects, name);
	CHECK(error);
	if (t == 0 && self->parent != 0) {
		t = self->parent->_->get_type_of(e__, self->parent, name);
		CHECK(error);
	}
	return t;
error:
	return 0;
}

static cc__Type *new_object(Ex **e__, cc__Scope *self)
{
	F("new_object");

	self->current_object = new_(cc__Type);
	CHECK(error1);
	return self->current_object;
error1:
	return 0;
}

static void name_object(Ex **e__, cc__Scope *self, cc__PreToken *name)
{
	F("name_object");
	(void)e__;
	(void)__func__;
	self->current_object->name = name;
	return;
}

static void end_object(Ex **e__, cc__Scope *self, cc__Type **old)
{
	F("end_object");
	cc__Type *o = 0;
	Hash_data *d;
	String s;

	
	STR(s, self->current_object->name->identifier);
	d = self->objects->_->set(e__, self->objects, &s,
		self->current_object, &o);
	CHECK(error1);
	self->current_object = 0;
	*old = o;
	return;
error1:
	return;
}

static void end_parameter(Ex **e__, cc__Scope *self, cc__Type **old)
{
	F("end_parameter");
	cc__Type *o;
	int32 i;	

	o = new_(cc__Type);
	o->name = new_(cc__PreToken);
	i = 0;
	while (self->current_object->name->identifier[i] != 0) i++;
	o->name->identifier = Gc__malloc(e__, i + 1);
	o->name->identifier[i] = 0;
	while (i > 0) {
		i--;
		o->name->identifier[i] = 
			self->current_object->name->identifier[i];
	}
	o->name->char_pos = self->current_object->name->char_pos;
	o->name->line = self->current_object->name->file;
	o->name->file = self->current_object->name->file;

	i = 0;
	while (i < self->current_object->data->nb_rett) {
		o->_->add_return_type(e__, o,
			 self->current_object->data->rett[i]);
		i++;
	}
	self->parent->current_object->_->add_parameter(e__,
		self->parent->current_object, o);
	CHECK(error1);

	end_object(e__, self, old);
	CHECK(error1);
	return;
error1:
	return;
}

static cc__Scope *new_scope(Ex **e__, cc__Scope *self)
{
	F("new_scope");
	cc__Scope *s;

	s = new_(cc__Scope);
	CHECK(error);
	s->parent = self;

	return s;
error:
	return 0;
}

static cc__Scope *end_scope(Ex **e__, cc__Scope *self)
{
	F("end_scope");
	cc__Scope *s;

	s = self->parent;
	delete_(self);
	CHECK(error);

	return s;
error:
	return 0;

}

static void add_return_type(Ex **e__, cc__Scope *self, cc__Type *type)
{
	F("add_return_type");

	if (self->current_object == 0) {
		self->_->new_object(e__, self);
		CHECK(error);
	}
	self->last_rett_id = type->id;
	self->current_object->_->add_return_type(e__, 
		self->current_object, type);
	CHECK(error);
	return;
error:
	return;
}

static void end_of_function(Ex **e__, cc__Scope *self)
{
	F("end_of_function");

	return;
error:
	return;
}

static void set_type_id(Ex **e__, cc__Scope *self, uint32 id)
{
	(void)e__;
	self->current_object->id = id;
}

static void dump_type(Ex **e__, cc__Type *type, uint32 ident)
{
	F("dump_scope");
	int32 i;

	ECHO("\n");

	while (ident > 0) {
		ident--;
		ECHO(" ");
	}
	ECHO(type->name->identifier);
	switch (type->id) {
	case CC__TOKEN_FUNCTION_TYPE:
		ECHO(" (func) ");
		break;
	default:
		ECHO(" (obj) ");
	}
	if (type->data != 0) {
		cc__Type_data *d = type->data;
		i = 0;
		ECHO(" SUB:");
		while (i < d->nb_sub) {
			ECHO(" ");
			ECHO(d->sub[i]->name->identifier);
			i++;
		}
		i = 0;
		ECHO(" RETT:");
		while (i < d->nb_rett) {
			ECHO(" ");
			ECHO(d->rett[i]->name->identifier);
			i++;
		}

		i = 0;
		ECHO(" PARAM:");
		while (i < d->nb_param) {
			cc__Type **t;
			uint32 j, k;
			ECHO(" ");
			ECHO(d->param[i]->name->identifier);
			ECHO(" = ");		
			t = d->param[i]->data->rett;
			j = d->param[i]->data->nb_rett;
			k = 0;
			while (k < j) {
				ECHO(" ");
				ECHO(t[k]->name->identifier);		
				k++;
			}
			ECHO(",");
			i++;
		}
	}
	CHECK(error);
	return;
error:
	return;
}

static void dump_scope(Ex **e__, cc__Scope *self, uint32 ident)
{
	F("dump_scope");
	Hash_iterate state;
		
	state.index = 0;
	state.data = 0;
	self->objects->_->iterate(e__, self->objects, &state);
	CHECK(error);

	while (state.data != 0) {
		cc__Type *t;

		t = state.data->data;
		dump_type(e__, t, ident);
		CHECK(error);
		self->objects->_->iterate(e__, self->objects, &state);
		CHECK(error);
	}

	if (self->parent != 0) {
		dump_scope(e__, self->parent, ident + 1);
		CHECK(error);
	}
	return;
error:
	return;
}

static void dump(Ex **e__, cc__Scope *self)
{
	F("dump");

	dump_scope(e__, self, 0);
	CHECK(error);
	return;
error:
	return;
}

INIT_EXPORT cc__Scope__class cc__Scope__  = {
	// Object
	(utf8*)"cc__Scope",
	sizeof(cc__Scope),
	0,
	__init__,
	__ctor__,
	__dtor__,
	hash_code,
	to_string,

	// Scope
	get_type_of,
	new_object,
	name_object,
	end_object,
	end_parameter,
	add_return_type,
	set_type_id,
	end_of_function,
	new_scope,
	end_scope,
	dump
};

