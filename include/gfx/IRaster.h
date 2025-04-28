/* "$Id: $"
 *
 *                          21 October MMX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#ifndef GFX__IRASTER_H
#define GFX__IRASTER_H

#include <lang/Object.h>

#define GFX__IRASTER_RGBA	0x0001
#define GFX__IRASTER_RGB	0x0002
#define GFX__IRASTER_GRAY	0x0004
#define GFX__IRASTER_MONO	0x0008
#define GFX__IRASTER_VECTOR	0x0010

struct gfx__Canvas;

typedef struct gfx__IRaster_info {
	void *handle;
	int32 x;
	int32 y;
	int32 w;
	int32 h;
	int32 scanline_width;
	uint32 pixel_format;
	uint8 *data;
	uint8 *mask;
	struct gfx__Canvas *vector;
} gfx__IRaster_info;

typedef struct gfx__IRaster {
	// Object
	struct gfx__IRaster__class *_;

	// Interface
	Object *__self__;
} gfx__IRaster;

typedef struct gfx__IRaster__class {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct Object__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, gfx__IRaster *self);
	void (*__dtor__)(Ex **e__, gfx__IRaster *self);
	uint32 (*hash_code)(Ex **e__, gfx__IRaster *self); 
	void (*to_string)(Ex **e__, gfx__IRaster *self, 
		struct String *out);

	// gfx__IRaster
	void (*get_info)(Ex **e__, gfx__IRaster *self,
		int32 x, int32 y, int32 w, int32 h, 
		gfx__IRaster_info *info);
	void (*flush)(Ex **e__, gfx__IRaster *self, gfx__IRaster_info *info);
} gfx__IRaster__class; 

#endif // GFX__IRASTER_H
