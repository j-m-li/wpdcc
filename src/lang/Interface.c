/* "$Id: $"
 *
 *                          15 June MMVIII PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <pal/pal.h>
#include <lang/Object.h>
#include <io/File.h>

static void __init__(Ex **e__)
{
	(void)e__;
	Interface__.super = &Object__;
	return;
}

static void __ctor__(Ex **e__, Interface *self)
{
	F("__ctor__");
	(void)e__;
	(void)self;	
	THROW(4, "Object is an Interface.");
	CHECK(error);
error:	
	return;
}

static void __dtor__(Ex **e__, Interface *self)
{
	F("__dtor__");

	(void)e__;
	(void)self;	
	THROW(4, "Object is an Interface.");
	CHECK(error);
error:	
	return;
}

static uint32 hash_code(Ex **e__, Interface *itf)
{
	F("hash_code");
	Object *self = itf->__self__;
	uint32 r;

	r = self->_->hash_code(e__, self);
	CHECK(error);
	return r;
error:
	return 0;
}


static void to_string(Ex **e__, Interface *itf, String *out)
{
	F("to_string");
	Object *self = itf->__self__;

	self->_->to_string(e__, self, out);
	CHECK(error);
error:
	return;
}


INIT_EXPORT Interface__class Interface__  = {
	(utf8*)"Interface",
	sizeof(Interface),
	0,
	__init__,
	__ctor__,
	__dtor__,
	hash_code,
	to_string
};
