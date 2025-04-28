/* "$Id: $"
 *
 *                          5 december 2009 PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <lang/Object.h>

#ifndef GFX__PATH_H
#define GFX__PATH_H

typedef struct gfx__Path_Metrics__struct {
	float32 width;
	float32 height;
	float32 descent;
} gfx__Path_Metrics;

typedef struct gfx__Path__struct {
	// Object
	struct gfx__Path__class_struct *_;

	// Font
} gfx__Path;

typedef struct gfx__Path__class_struct {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct Object__class_struct *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, gfx__Path *self);
	void (*__dtor__)(Ex **e__, gfx__Path *self);
	uint32 (*hash_code)(Ex **e__, gfx__Path *self); 
	void (*to_string)(Ex **e__, gfx__Path *self,
		struct String *out);

	// Font
} gfx__Path__class; 

EXPORT gfx__Path__class gfx__Path__;

#endif // GFX__PATH_H

