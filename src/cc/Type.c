/* "$Id: $"
 *
 *                          10 October MMIX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <cc/Type.h>

static void __init__(Ex **e__)
{
	(void)e__;
	cc__Type__.super = &Object__;
	return;
}


static void __ctor__(Ex **e__, cc__Type *self)
{
	F("__ctor__");

	(void)__func__;
	(void)e__;

	self->id = 0;
	self->data = 0;
	self->name = 0;
	self->to_basic_block = 0;
	return;
}

static void delete_data(Ex **e__, cc__Type_data *data)
{
	F("delete_data");

	Gc__free(e__, data->sub);
	CHECK(error1);
error1:
	Gc__free(e__, data->rett);
	CHECK(error2);
error2:
	Gc__free(e__, data->param);
	CHECK(error3);
error3:
	Gc__free(e__, data->init_value);
	CHECK(error4);
error4:
	return;
}

static void __dtor__(Ex **e__, cc__Type *self)
{
	F("__dtor__");
	
	if (self->data != 0) delete_data(e__, self->data);
	CHECK(error1);
error1:
	if (self->name != 0) delete_(self->name);
	CHECK(error2);
error2:
	return;
}

static uint32 hash_code(Ex **e__, cc__Type *self)
{
	(void)e__;

#ifdef _MSC_VER
	return *((uint32*)&self) >> 4;
#else 
	return (uint32)self;
#endif

} 

static void to_string(Ex **e__, cc__Type *self, String *out)
{
	F("to_string");
	String s;

	STR(s, self->_->__name__);
	out->_->add(e__, out, &s);
	CHECK(error);
error:
	return;
}

static cc__Type_data *new_data(Ex **e__)
{
	F("new_data");
	cc__Type_data *data;

	data = Gc__malloc(e__, sizeof(cc__Type_data));
	CHECK(error);
	data->sub = 0;
	data->nb_sub = 0;
	data->rett = 0;
	data->nb_rett = 0;
	data->param = 0;
	data->nb_param = 0;
	data->nb_param = 0;
	data->init_value = 0;
	data->init_value_size = 0;
	return data;
error:
	return 0;
}

static void init(Ex **e__, cc__Type *self, uint32 id, cc__PreToken *name)
{
	F("init");

	(void)__func__;
	(void)e__;

	self->id = id;
	self->name = name;
	return;
}

static void add_sub_type(Ex **e__, cc__Type *self, cc__Type *sub)
{
	F("add_sub_type");
	cc__Type **ns;
	cc__Type_data *data;

	if (self->data == 0) {
		self->data = new_data(e__);
		CHECK(error1);
	}
	data = self->data;

	if ((data->nb_sub & 0x7) == 0) {
		int32 i = data->nb_sub;
		ns = Gc__malloc(e__,  (i + 8) * sizeof(cc__Type*));
		CHECK(error1);
		while (i > 0) {
			i--;
			ns[i] = data->sub[i];
		}
		Gc__free(e__, data->sub);
		CHECK(error2);
		data->sub = ns;
	}
	data->sub[data->nb_sub] = sub;
	data->nb_sub++;
	return;
error2:
	Gc__free(e__, ns);
error1:
	return;	
}

static void add_parameter(Ex **e__, cc__Type *self, cc__Type *param)
{
	F("add_parameter");
	cc__Type **ns;
	cc__Type_data *data;

	if (self->data == 0) {
		self->data = new_data(e__);
		CHECK(error1);
	}
	data = self->data;

	if ((data->nb_param & 0x7) == 0) {
		int32 i = data->nb_param;
		ns = Gc__malloc(e__,  (i + 8) * sizeof(cc__Type*));
		CHECK(error1);
		while (i > 0) {
			i--;
			ns[i] = data->param[i];
		}
		Gc__free(e__, data->param);
		CHECK(error2);
		data->param = ns;
	}
	data->param[data->nb_param] = param;
	data->nb_param++;
	return;
error2:
	Gc__free(e__, ns);
error1:
	return;	
}
static void add_return_type(Ex **e__, cc__Type *self, cc__Type *rett)
{
	F("add_return_type");
	cc__Type **ns;
	cc__Type_data *data;

	if (self->data == 0) {
		self->data = new_data(e__);
		CHECK(error1);
	}
	data = self->data;

	if ((data->nb_rett & 0x7) == 0) {
		int32 i = data->nb_rett;
		ns = Gc__malloc(e__,  (i + 8) * sizeof(cc__Type*));
		CHECK(error1);
		while (i > 0) {
			i--;
			ns[i] = data->rett[i];
		}
		Gc__free(e__, data->rett);
		CHECK(error2);
		data->rett = ns;
	}
	data->rett[data->nb_rett] = rett;
	data->nb_rett++;
	return;
error2:
	Gc__free(e__, ns);
error1:
	return;	
}

INIT_EXPORT cc__Type__class cc__Type__  = {
	// Object
	(utf8*)"cc__Type",
	sizeof(cc__Type),
	0,
	__init__,
	__ctor__,
	__dtor__,
	hash_code,
	to_string,

	// Type
	init,
	add_sub_type,
	add_parameter,
	add_return_type
};

