/* "$Id: $"
 *
 *                          27 July MMVIII PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <cc/PreMacro.h>

static void __init__(Ex **e__)
{
	(void)e__;

	cc__PreMacro__.super = &Object__;
	return;
}


static void __ctor__(Ex **e__, cc__PreMacro *self)
{
	F("__ctor__");

	(void)__func__;
	(void)e__;

	self->body = 0;
	self->nb_parameters = 0;
	self->parameters = 0;
	return;
}

static void __dtor__(Ex **e__, cc__PreMacro *self)
{
	F("__dtor__");
	int32 i;

	(void)__func__;

	delete_(self->body);
	for (i = self->nb_parameters - 1; i >= 0; i--) {
		Gc__free(e__, self->parameters[i]);
	}
	Gc__free(e__, self->parameters);
	return;
}

static uint32 hash_code(Ex **e__, cc__PreMacro *self)
{
	(void)e__;
#ifdef _MSC_VER
	return *((uint32*)&self) >> 4;
#else 
	return (uint32)self;
#endif

} 

static void to_string(Ex **e__, cc__PreMacro *self, String *out)
{
	F("to_string");
	String s;

	STR(s, self->_->__name__);
	out->_->add(e__, out, &s);
	CHECK(error);
error:
	return;
}

static void set(Ex **e__, cc__PreMacro *self, cc__PreTokens *body,
	utf8 **parameters, int32 nb_parameters, int8 is_object,
	uint32 file, uint32 line, uint32 char_pos)
{
	(void)e__;

	self->body = body;
	self->char_pos = char_pos;
	self->file = file;
	self->is_object = is_object;
	self->line = line;
	self->nb_parameters = nb_parameters;
	self->parameters = parameters;
	return;
}

INIT_EXPORT cc__PreMacro__class cc__PreMacro__  = {
	// Object
	(utf8*)"cc__PreMacro",
	sizeof(cc__PreMacro),
	0,
	__init__,
	__ctor__,
	__dtor__,
	hash_code,
	to_string,

	// PreMacros
	set
};

