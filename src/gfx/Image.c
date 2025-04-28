/* "$Id: $"
 *
 *                          12 september MMX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */


#include <gfx/Image.h>

#include "stb_image.c"

static gfx__IRaster__class gfx__IRaster__;

static void __init__(Ex **e__)
{
	(void)e__;
	gfx__Image__.super = &Object__;
	INIT_INTERFACE(gfx__IRaster, gfx__Image);
	return;
}


static void __ctor__(Ex **e__, gfx__Image *self)
{
	F("__ctor__");

	(void)e__;
	(void)__func__;	
	self->w = 0;
	self->h = 0;
	self->line_size = 0;
	self->rgba = 0;
	return;
}


static void __dtor__(Ex **e__, gfx__Image *self)
{
	F("__dtor__");

	if (self->rgba) Gc__free(e__, self->rgba);
	CHECK(error1);
error1:
	return;
}

static uint32 hash_code(Ex **e__, gfx__Image *self)
{
	(void)e__;
#ifdef _MSC_VER
	return *((uint32*)&self) >> 4;
#else 
	return (uint32)self >> 4;
#endif
} 

static void to_string(Ex **e__, gfx__Image *self, String *out)
{
	F("to_string");
	String s;

	STR(s, self->_->__name__);
	out->_->add(e__, out, &s);
	CHECK(error);
error:
	return;
}

static void size(Ex **e__, gfx__Image *self, int32 x, int32 y,
		int32 w, int32 h)
{
	F("size");

	(void)x;
	(void)y;
	(void)w;
	(void)h;

	if (self->rgba) Gc__free(e__, self->rgba);
	CHECK(error1);

	return;
error1:
	return;
}

static void load(Ex **e__, gfx__Image *self, String *name)
{
	F("load");
	utf8 *n;
	int32 c;
	(void)__func__;
	(void)self;
	(void)name;
	(void)e__;

	n = name->_->get_cstr(e__, name);
	self->rgba = stbi_load(n, &self->w, &self->h, &c, 4);
}

static void save(Ex **e__, gfx__Image *self, String *name)
{
	F("save");
	(void)__func__;
	(void)self;
	(void)name;
	(void)e__;

}

static void gfx__i_raster(Ex **e__, gfx__Image *self, gfx__IRaster *out)
{
	(void)e__;
	out->_ = &gfx__IRaster__;
	out->__self__ = (Object*)self;	
}

static void get_info(Ex **e__, gfx__IRaster *itf, int32 x, int32 y, int32 w, 
	int32 h, gfx__IRaster_info *info)
{
	gfx__Image *self = (gfx__Image*)itf->__self__;

	(void)e__;

	info->vector = 0;
	info->pixel_format = GFX__IRASTER_RGBA;
	info->x = x;
	info->y = y;
	info->w = w;
	info->h = h;
	info->data = self->rgba;
}

static void flush(Ex **e__, gfx__IRaster *itf, gfx__IRaster_info *info)
{
	gfx__Image *self = (gfx__Image*)itf->__self__;
	(void)e__;
	(void)info;
	(void)self;
}


INIT_EXPORT gfx__Image__class gfx__Image__  = {
	// Object
	(utf8*)"gfx__Image",
	sizeof(gfx__Image),
	0,
	__init__,
	__ctor__,
	__dtor__,
	hash_code,
	to_string,

	// Image
	size,
	load,
	save,
	gfx__i_raster,
	get_info,
	flush
};

static gfx__IRaster__class gfx__IRaster__  = {
	// Object
	0, 0, 0, 0, 0, 0, 0, 0,

	// IPaint
	get_info,
	flush
};
