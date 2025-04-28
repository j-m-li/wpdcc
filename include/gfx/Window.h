/* "$Id: $"
 *
 *                          21 october MMX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <lang/Object.h>

#ifndef GFX__WINDOW_H
#define GFX__WINDOW_H

#include <gfx/IEventHandler.h>
#include <gfx/Canvas.h>
#include <gfx/Box.h>

typedef struct gfx__Window {
	// Object
	struct gfx__Window__class *_;

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

	// Window
	gfx__Box *selection_start_box;
	gfx__Box *selection_end_box;
	int32 selection_start_index;
	int32 selection_end_index;
	gfx__Style *style_factory;
	gfx__Event *evt;
	gfx__Event *evt2;
} gfx__Window;

typedef struct gfx__Window__class {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct gfx__Box__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, gfx__Window *self);
	void (*__dtor__)(Ex **e__, gfx__Window *self);
	uint32 (*hash_code)(Ex **e__, gfx__Window *self); 
	void (*to_string)(Ex **e__, gfx__Window *self,
		struct String *out);

	// Box
	void (*gfx__i_event_handler)(Ex **e__, gfx__Window *self, 
		gfx__IEventHandler *out);
	void (*event)(Ex **e__, gfx__IEventHandler *self, 
		struct gfx__Event *evt);

	// Window
	gfx__Box *(*insert_box)(Ex **e__, gfx__Window *self, uint32 display);
	void (*insert_text)(Ex **e__, gfx__Window *self, String *text);
	void (*size)(Ex **e__, gfx__Window *self, int32 x, int32 y, 
		int32 w, int32 h);
} gfx__Window__class; 

EXPORT gfx__Window__class gfx__Window__;

#endif // GFX__WINDOW_H

