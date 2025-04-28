/* "$Id: $"
 *
 *                          21 october MMX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <lang/Object.h>

#ifndef GFX__EVENT_H
#define GFX__EVENT_H

#include <gfx/Canvas.h>

#define GFX__EVENT_NONE			0x00000000
#define GFX__EVENT_CLASS_CUSTOM		0x00010000
#define GFX__EVENT_CLASS_DRAW		0x00020000
#define GFX__EVENT_CLASS_KEYBOARD	0x00040000
#define GFX__EVENT_CLASS_MOUSE		0x00080000
#define GFX__EVENT_CLASS_CHANGE		0x00100000
#define GFX__EVENT_CLASS_DND		0x00200000
#define GFX__EVENT_CLASS_CUT_PASTE	0x00400000

#define GFX__EVENT_DRAW		(GFX__EVENT_CLASS_DRAW + 1)
#define GFX__EVENT_INIT_STYLE	(GFX__EVENT_CLASS_DRAW + 2)
#define GFX__EVENT_MEASURE	(GFX__EVENT_CLASS_DRAW + 3)
#define GFX__EVENT_FORMAT	(GFX__EVENT_CLASS_DRAW + 4)

#define GFX__EVENT_KEYPRESS	(GFX__EVENT_CLASS_KEYBOARD + 1)
#define GFX__EVENT_KEYDOWN	(GFX__EVENT_CLASS_KEYBOARD + 2)	
#define GFX__EVENT_KEYUP	(GFX__EVENT_CLASS_KEYBOARD + 3)	

#define GFX__EVENT_INSERT_TEXT	(GFX__EVENT_CLASS_CHANGE + 1)


struct gfx__Box;
struct gfx__Window;

typedef struct gfx__Event_rect
{
	int32 x;
	int32 y;
	int32 w;
	int32 h;
} gfx__Event_rect;

typedef struct gfx__Event {
	// Object
	struct gfx__Event__class *_;

	// Event
	gfx__Event_rect r;
	uint32 id;

	gfx__Canvas *canvas;
	struct gfx__Box *containing_block;
	String *text;

	struct gfx__Window *window;

	uint8 handled;
	uint8 stop_propagation;
} gfx__Event;

typedef struct gfx__Event__class {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct Object__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, gfx__Event *self);
	void (*__dtor__)(Ex **e__, gfx__Event *self);
	uint32 (*hash_code)(Ex **e__, gfx__Event *self); 
	void (*to_string)(Ex **e__, gfx__Event *self,
		struct String *out);

	// Event
} gfx__Event__class; 

EXPORT gfx__Event__class gfx__Event__;

#endif // GFX__EVENT_H

