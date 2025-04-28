/* "$Id: $"
 *
 *                          10 december MMX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <lang/Object.h>

#ifndef GFX__LINE_H
#define GFX__LINE_H

#include <gfx/Style.h>

typedef struct gfx__Line {
	// Object
	struct gfx__Line__class *_;

	// Line
	int32 left;
	int32 top;
	int32 width;
	int32 height;
	int32 descent;
	
	uint32 flags;
	gfx__Style_css *style;
	int32 start;
	struct gfx__Box *parent;
} gfx__Line;

typedef struct gfx__Line__class {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct Object__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, gfx__Line *self);
	void (*__dtor__)(Ex **e__, gfx__Line *self);
	uint32 (*hash_code)(Ex **e__, gfx__Line *self); 
	void (*to_string)(Ex **e__, gfx__Line *self,
		struct String *out);

	// Line
	void (*gfx__i_event_handler)(Ex **e__, gfx__Line *self, 
		gfx__IEventHandler *out);
	void (*event)(Ex **e__, gfx__IEventHandler *self, 
		struct gfx__Event *evt);
} gfx__Line__class; 


EXPORT gfx__Line__class gfx__Line__;

#endif // GFX__LINE_H

