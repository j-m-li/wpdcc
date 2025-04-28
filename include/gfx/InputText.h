/* "$Id: $"
 *
 *                          21 october MMX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <lang/Object.h>

#ifndef gfx__InputText_H
#define gfx__InputText_H

#include <gfx/IEventHandler.h>
#include <gfx/Canvas.h>
#include <gfx/Box.h>

typedef struct gfx__InputText {
	// Object
	struct gfx__InputText__class *_;

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
	gfx__Box *parent;
	struct gfx__Line **lines;
	int32 nb_lines;

	// InputText
} gfx__InputText;

typedef struct gfx__InputText__class {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct gfx__Box__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, gfx__InputText *self);
	void (*__dtor__)(Ex **e__, gfx__InputText *self);
	uint32 (*hash_code)(Ex **e__, gfx__InputText *self); 
	void (*to_string)(Ex **e__, gfx__InputText *self,
		struct String *out);

	// Box
	void (*gfx__i_event_handler)(Ex **e__, gfx__InputText *self, 
		gfx__IEventHandler *out);
	void (*event)(Ex **e__, gfx__IEventHandler *self, 
		struct gfx__Event *evt);

	// Window
} gfx__InputText__class; 

EXPORT gfx__InputText__class gfx__InputText__;

#endif // gfx__InputText_H

