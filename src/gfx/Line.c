/* "$Id: $"
 *
 *                          10 december MMX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */


#include <gfx/Line.h>
#include <gfx/IEventHandler.h>
#include <gfx/Canvas.h>
#include <gfx/Window.h>

static gfx__IEventHandler__class gfx__IEventHandler__;


static void __init__(Ex **e__)
{
	(void)e__;
	gfx__Line__.super = &Object__;
	INIT_INTERFACE(gfx__IEventHandler, gfx__Line);

	return;
}

static void __ctor__(Ex **e__, gfx__Line *self)
{
	F("__ctor__");

	self->flags = 0;
	self->style = 0;
	self->descent = 0;
	self->height = 0;
	self->width = 0;
	CHECK(error1);
	return;	
error1:
	return;
}


static void __dtor__(Ex **e__, gfx__Line *self)
{
	F("__dtor__");
	(void)e__;
	(void)self;
	(void)__func__;
	return;
}

static uint32 hash_code(Ex **e__, gfx__Line *self)
{
#ifdef _MSC_VER
	return *((uint32*)&self) >> 4;
	e__;
#else 
	return (uint32)self >> 4;
#endif
} 

static void to_string(Ex **e__, gfx__Line *self, String *out)
{
	F("to_string");
	String s;

	STR(s, self->_->__name__);
	out->_->add(e__, out, &s);
	CHECK(error);
error:
	return;
}

static void gfx__i_event_handler(Ex **e__, gfx__Line *self, 
	gfx__IEventHandler *out)
{
	(void)e__;
	out->_ = &gfx__IEventHandler__;
	out->__self__ = (Object*)self;	
}


static void default_handler(Ex **e__, gfx__Line *self, gfx__Event *evt)
{
	switch (evt->id) {
	case GFX__EVENT_DRAW:
//		draw(e__, self, evt);
		break;
	case GFX__EVENT_MEASURE:
//		measure(e__, self, evt);
		break;
	case GFX__EVENT_FORMAT:
//		format(e__, self, evt);
		break;
	default:
		evt->handled = 0;
	}
}

static void event(Ex **e__, gfx__IEventHandler *itf, gfx__Event *evt)
{
	gfx__Line *self;
	uint32 d;

	self = (gfx__Line*)itf->__self__;
	default_handler(e__, self, evt);
}


INIT_EXPORT gfx__Line__class gfx__Line__  = {
	// Object
	(utf8*)"gfx__Line",
	sizeof(gfx__Line),
	0,
	__init__,
	__ctor__,
	__dtor__,
	hash_code,
	to_string,

	// Box
	gfx__i_event_handler,
	event
};


static gfx__IEventHandler__class gfx__IEventHandler__  = {
	// Object
	0, 0, 0, 0, 0, 0, 0, 0,

	// IEventHandler
	event
};
