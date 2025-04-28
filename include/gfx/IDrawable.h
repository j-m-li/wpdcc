/* "$Id: $"
 *
 *                          21 October MMX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#ifndef GFX__IDRAWABLE_H
#define GFX__IDRAWABLE_H

#include <lang/Object.h>
#include <gfx/IRaster.h>

typedef struct gfx__IDrawable {
	// Object
	struct gfx__IDrawable__class *_;

	// Interface
	Object *__self__;
} gfx__IDrawable;

struct gfx__IRaster;

typedef struct gfx__IDrawable__class {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct Object__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, gfx__IDrawable *self);
	void (*__dtor__)(Ex **e__, gfx__IDrawable *self);
	uint32 (*hash_code)(Ex **e__, gfx__IDrawable *self); 
	void (*to_string)(Ex **e__, gfx__IDrawable *self, 
		struct String *out);

	// gfx__IDrawable
	void (*draw)(Ex **e__, gfx__IDrawable *self,
		struct gfx__IRaster *raster);
} gfx__IDrawable__class; 

#endif // GFX__IDRAWABLE_H
