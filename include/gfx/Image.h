/* "$Id: $"
 *
 *                          5 december MMIX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <lang/Object.h>

#ifndef GFX__IMAGE_H
#define GFX__IMAGE_H

#include <gfx/IRaster.h>

typedef struct gfx__Image {
	// Object
	struct gfx__Image__class *_;

	// Image
	int32 w;
	int32 h;

	uint8 *rgba;
	int32 line_size;
} gfx__Image;

typedef struct gfx__Image__class {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct Object__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, gfx__Image *self);
	void (*__dtor__)(Ex **e__, gfx__Image *self);
	uint32 (*hash_code)(Ex **e__, gfx__Image *self); 
	void (*to_string)(Ex **e__, gfx__Image *self,
		struct String *out);

	// Image
	void (*size)(Ex **e__, gfx__Image *self, int32 x, int32 y,
		int32 w, int32 h);

	void (*load)(Ex **e__, gfx__Image *self, String *name);
	void (*save)(Ex **e__, gfx__Image *self, String *name);

	// IRaster
	void (*gfx__i_raster)(Ex **e__, gfx__Image *self, gfx__IRaster *out);
	void (*get_info)(Ex **e__, gfx__IRaster *itf, 
		int32 x, int32 y, int32 w, int32 h, 
		gfx__IRaster_info *info);
	void (*flush)(Ex **e__, gfx__IRaster *itf, gfx__IRaster_info *info);
} gfx__Image__class; 

EXPORT gfx__Image__class gfx__Image__;

#endif // GFX__IMAGE_H

