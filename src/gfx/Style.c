/* "$Id: $"
 *
 *                          5 december MMIX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */


#include <gfx/Style.h>
#include <gfx/Box.h>
#include <gfx/IEventHandler.h>
#include <gfx/IRaster.h>

static gfx__IEventHandler__class gfx__IEventHandler__;

static void __init__(Ex **e__)
{
	(void)e__;
	gfx__Style__.super = &Object__;
	INIT_INTERFACE(gfx__IEventHandler, gfx__Style);

	return;
}

static void __ctor__(Ex **e__, gfx__Style *self)
{
	F("__ctor__");

	CHECK(error1);
	return;	
error1:
	return;
}


static void __dtor__(Ex **e__, gfx__Style *self)
{
	F("__dtor__");
	(void)e__;
	(void)self;
	(void)__func__;
	return;
}

static uint32 hash_code(Ex **e__, gfx__Style *self)
{
	(void)e__;
#ifdef _MSC_VER
	return *((uint32*)&self) >> 4;
#else 
	return (uint32)self >> 4;
#endif
} 

static void to_string(Ex **e__, gfx__Style *self, String *out)
{
	F("to_string");
	String s;

	STR(s, self->_->__name__);
	out->_->add(e__, out, &s);
	CHECK(error);
error:
	return;
}

static void create(Ex **e__, gfx__Style *self,
		uint32 display, struct gfx__Box *box)
{
	uint32 i;
	uint8 *p;
	uint32 *u;
	gfx__Style_css *s;

	s = box->style = Gc__malloc(e__, sizeof(gfx__Style_css));
	
	p = (uint8*)box->style;
	i = sizeof(gfx__Style_css);
	while (i > 0) {
		i--;
		p[i] = 0;
	}		
	s->display = display;
	s->background_color = RGBA(0xFF, 0xFF, 0xFF, 0xFF);
	s->color = RGBA(0x00, 0x00, 0x00, 0xFF);
	s->width = -1;
	s->height = -1;
}


INIT_EXPORT gfx__Style__class gfx__Style__  = {
	// Object
	(utf8*)"gfx__Style",
	sizeof(gfx__Style),
	0,
	__init__,
	__ctor__,
	__dtor__,
	hash_code,
	to_string,

	// Style
	create
};
	