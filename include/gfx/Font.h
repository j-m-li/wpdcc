/* "$Id: $"
 *
 *                          5 december MMIX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <lang/Object.h>

#ifndef GFX__FONT_H
#define GFX__FONT_H

typedef struct gfx__Font_metrics {
	float32 width;
	float32 height;
	float32 descent;
	float32 advance;
	float32 left_side_bearing;
	float32 line_gap;
} gfx__Font_metrics;

typedef struct gfx__Font_glyph {
	int32 width;
	int32 height;
	int32 descent;
	int32 y0;
	uint8 *data;
	int32 xoff;
	int32 yoff;
	float32 left_side_bearing;
	float32 advance;
} gfx__Font_glyph;

typedef struct gfx__Font {
	// Object
	struct gfx__Font__class *_;

	// Font
	float32 size;
} gfx__Font;

typedef struct gfx__Font__class {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct Object__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, gfx__Font *self);
	void (*__dtor__)(Ex **e__, gfx__Font *self);
	uint32 (*hash_code)(Ex **e__, gfx__Font *self); 
	void (*to_string)(Ex **e__, gfx__Font *self,
		struct String *out);

	// Font
	void (*load)(Ex **e__, gfx__Font *self, String *name);
	void (*get_glyph)(Ex **e__, gfx__Font *self, 
		int32 unicode, float32 size, gfx__Font_glyph *g);
	void (*get_metrics)(Ex **e__, gfx__Font *self, 
		int32 unicode, float32 size, gfx__Font_metrics *m);
	void (*release_glyph)(Ex **e__, gfx__Font *self, gfx__Font_glyph *g);

} gfx__Font__class; 

EXPORT gfx__Font__class gfx__Font__;

#endif // GFX__FONT_H

