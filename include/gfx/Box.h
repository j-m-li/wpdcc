/* "$Id: $"
 *
 *                          28 october MMX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <lang/Object.h>

#ifndef GFX__BOX_H
#define GFX__BOX_H

#include <gfx/Style.h>
#include <gfx/Line.h>

#define GFX__BOX_DAMAGE_CHILD		0x00000001
#define GFX__BOX_DAMAGE_ALL		0x00000002


typedef struct gfx__Box {
	// Object
	struct gfx__Box__class *_;

	// Box
	int32 left;
	int32 top;
	int32 width;
	int32 height;
	int32 descent;
	int32 min_width;
	int32 max_width;
	
	uint32 flags;
	gfx__Style_css *style;
	utf8 *content;
	int32 length;
	struct gfx__Box *parent;
	struct gfx__Line **lines;
	int32 nb_lines;
} gfx__Box;

typedef struct gfx__Box__class {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct Object__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, gfx__Box *self);
	void (*__dtor__)(Ex **e__, gfx__Box *self);
	uint32 (*hash_code)(Ex **e__, gfx__Box *self); 
	void (*to_string)(Ex **e__, gfx__Box *self,
		struct String *out);

	// Box
	void (*gfx__i_event_handler)(Ex **e__, gfx__Box *self, 
		gfx__IEventHandler *out);
	void (*event)(Ex **e__, gfx__IEventHandler *self, 
		struct gfx__Event *evt);
} gfx__Box__class; 


EXPORT gfx__Box__class gfx__Box__;

EXPORT void gfx__Box__draw_background_and_border(
	Ex **e__, gfx__Box *self, gfx__Event *evt);

#endif // GFX__BOX_H

