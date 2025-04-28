/* "$Id: $"
 *
 *                          21 October MMX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#ifndef GFX__IEVENTHANDLER_H
#define GFX__IEVENTHANDLER_H

#include <lang/Object.h>
#include <gfx/Event.h>


typedef struct gfx__IEventHandler {
	// Object
	struct gfx__IEventHandler__class *_;

	// Interface
	Object *__self__;
} gfx__IEventHandler;

struct gfx__Event;
struct gfx__IRaster;

typedef struct gfx__IEventHandler__class {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct Object__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, gfx__IEventHandler *self);
	void (*__dtor__)(Ex **e__, gfx__IEventHandler *self);
	uint32 (*hash_code)(Ex **e__, gfx__IEventHandler *self); 
	void (*to_string)(Ex **e__, gfx__IEventHandler *self, 
		struct String *out);

	// gfx__IEventHandler
	void (*event)(Ex **e__, gfx__IEventHandler *self, 
		struct gfx__Event *evt);
} gfx__IEventHandler__class; 

#endif // GFX__IEVENTHANDLER_H
