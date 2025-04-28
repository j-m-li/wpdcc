/* "$Id: $"
 *
 *                          5 december MMIX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */


#include <gfx/Event.h>

static void __init__(Ex **e__)
{
	(void)e__;
	gfx__Event__.super = &Object__;

	return;
}

static void __ctor__(Ex **e__, gfx__Event *self)
{
	F("__ctor__");
	
	(void)e__;
	(void)__func__;
	self->r.x = 0;
	self->r.y = 0;
	self->r.w = 0;
	self->r.h = 0;
	self->id = 0;
	self->canvas = 0;
	return;	
}


static void __dtor__(Ex **e__, gfx__Event *self)
{
	F("__dtor__");
	(void)e__;
	(void)self;
	(void)__func__;
	return;
}

static uint32 hash_code(Ex **e__, gfx__Event *self)
{
#ifdef _MSC_VER
	return *((uint32*)&self) >> 4;
	e__;
#else 
	return (uint32)self >> 4;
#endif
} 

static void to_string(Ex **e__, gfx__Event *self, String *out)
{
	F("to_string");
	String s;

	STR(s, self->_->__name__);
	out->_->add(e__, out, &s);
	CHECK(error);
error:
	return;
}

INIT_EXPORT gfx__Event__class gfx__Event__  = {
	// Object
	(utf8*)"gfx__Event",
	sizeof(gfx__Event),
	0,
	__init__,
	__ctor__,
	__dtor__,
	hash_code,
	to_string,

	// Event
	
};


