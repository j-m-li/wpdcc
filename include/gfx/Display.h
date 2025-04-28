/* "$Id: $"
 *
 *                          5 december MMIX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <lang/Object.h>

#ifndef GFX__DISPLAY_H
#define GFX__DISPLAY_H

#include <gfx/Canvas.h>
#include <gfx/IRaster.h>
#include <gfx/IEventHandler.h>

typedef struct gfx__Display {
	// Object
	struct gfx__Display__class *_;

	// Display
	int32 x;
	int32 y;
	int32 w;
	int32 h;

	uint8 *rgba;
	uint8 *mask;
	int32 line_size;
	void *gh_wnd; 
	void *gh_dc; 
	void *gh_rc;

	Ex **e__;
	gfx__IEventHandler root;
	gfx__Event *event;
	gfx__Canvas *canvas;
} gfx__Display;

typedef struct gfx__Display__class {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct Object__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, gfx__Display *self);
	void (*__dtor__)(Ex **e__, gfx__Display *self);
	uint32 (*hash_code)(Ex **e__, gfx__Display *self); 
	void (*to_string)(Ex **e__, gfx__Display *self,
		struct String *out);

	// Display
	void (*size)(Ex **e__, gfx__Display *self, int32 x, int32 y,
		int32 w, int32 h);
	void (*show)(Ex **e__, gfx__Display *self, uint8 state);
	void (*process_events)(Ex **e__, gfx__Display *self);
	void (*run)(Ex **e__, gfx__Display *self);
	void (*draw)(Ex **e__, gfx__Display *self);
	void (*set_root)(Ex **e__, gfx__Display *self, 
		gfx__IEventHandler *event);

	void (*gfx__i_raster)(Ex **e__, gfx__Display *self, gfx__IRaster *out);
	void (*get_info)(Ex **e__, gfx__IRaster *itf, 
		int32 x, int32 y, int32 w, int32 h, 
		gfx__IRaster_info *info);
	void (*flush)(Ex **e__, gfx__IRaster *itf, gfx__IRaster_info *info);
} gfx__Display__class; 

EXPORT gfx__Display__class gfx__Display__;

EXPORT void *gfx__Display__h_instance;

#endif // GFX__DISPLAY_H

