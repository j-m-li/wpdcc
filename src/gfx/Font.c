/* "$Id: $"
 *
 *                          5 december MMIX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */


#include <gfx/Font.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

static void __init__(Ex **e__)
{
	(void)e__;
	gfx__Font__.super = &Object__;

	return;
}

static void __ctor__(Ex **e__, gfx__Font *self)
{
	F("__ctor__");
	
	(void)e__;
	(void)__func__;
	
	self->size = 10;
	//self->size = 72;
	return;	
}


static void __dtor__(Ex **e__, gfx__Font *self)
{
	F("__dtor__");
	(void)e__;
	(void)self;
	(void)__func__;
	return;
}

static uint32 hash_code(Ex **e__, gfx__Font *self)
{
#ifdef _MSC_VER
	return *((uint32*)&self) >> 4;
	e__;
#else 
	return (uint32)self >> 4;
#endif
} 

static void to_string(Ex **e__, gfx__Font *self, String *out)
{
	F("to_string");
	String s;

	STR(s, self->_->__name__);
	out->_->add(e__, out, &s);
	CHECK(error);
error:
	return;
}

static stbtt_fontinfo font;

static uint8 tuffy[] =
#include "tuffy.h"

static void load(Ex **e__, gfx__Font *self, String *name)
{
	stbtt_InitFont(&font, tuffy, stbtt_GetFontOffsetForIndex(tuffy,0));

}

#define SIZE_ADJUST 1.63

static void get_glyph(Ex **e__, gfx__Font *self, int32 unicode,
	float32 size, gfx__Font_glyph *g)
{
	float32 scale;
	int32 a, lsb, x0, y0, x1, y1;
	int32 ascent, descent, line_gap;


	size = size * SIZE_ADJUST;

	stbtt_GetFontVMetrics(&font, &ascent, &descent, &line_gap);
	scale = stbtt_ScaleForPixelHeight(&font, size);
	g->descent = descent * scale;
	g->data = stbtt_GetCodepointBitmap(&font, 0, scale, 
		unicode, &g->width, &g->height, &g->xoff, &g->yoff);
 	stbtt_GetCodepointHMetrics(&font, unicode, &a, &lsb);
	stbtt_GetCodepointBitmapBox(&font, unicode, scale, scale, &x0, &g->y0,
		&x1, &y1);
	g->height = y1 - g->y0;
	g->width = x1 - x0;
	g->advance = a * scale;
	g->left_side_bearing = lsb * scale;
}

static void get_metrics(Ex **e__, gfx__Font *self, int32 unicode,
	float32 size, gfx__Font_metrics *g)
{
	float32 scale;
	int32 a, lsb;
	int32 ascent, descent, line_gap;

	size = size * SIZE_ADJUST;
	stbtt_GetFontVMetrics(&font, &ascent, &descent, &line_gap);
	scale = stbtt_ScaleForPixelHeight(&font, size);
	g->line_gap = line_gap * scale;
	g->descent = -descent * scale;
	g->height = g->descent + ascent * scale;
 	stbtt_GetCodepointHMetrics(&font, unicode, &a, &lsb);
	g->width = g->advance = a * scale;
	g->left_side_bearing = lsb * scale;
}

static void release_glyph(Ex **e__, gfx__Font *self, gfx__Font_glyph *g)
{
	free(g->data);
}

INIT_EXPORT gfx__Font__class gfx__Font__  = {
	// Object
	(utf8*)"gfx__Font",
	sizeof(gfx__Font),
	0,
	__init__,
	__ctor__,
	__dtor__,
	hash_code,
	to_string,

	// Font
	load,
	get_glyph,
	get_metrics,
	release_glyph
};


