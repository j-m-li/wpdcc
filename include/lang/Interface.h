/* "$Id: $"
 *
 *                          29 July MMIX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#ifndef LANG__INTERFACE_H
#define LANG__INTERFACE_H

#include <lang/Object.h>

#define INIT_INTERFACE(t, o) { \
	if (t##__.super == 0) { \
		t##__.__name__ = o##__.__name__; \
		t##__.__size__ = 0; \
		t##__.super = Interface__.super; \
		t##__.__init__ = (void (*)(Ex **e__))Interface__.__init__; \
		t##__.__ctor__ = (void (*)(Ex **, t *)) \
			Interface__.__ctor__; \
		t##__.__dtor__ = (void (*)(Ex **, t *)) \
			Interface__.__dtor__; \
		t##__.hash_code = (uint32 (*)(Ex **, t *)) \
			Interface__.hash_code; \
		t##__.to_string = \
			(void (*)(Ex **, t *, struct String *)) \
			Interface__.to_string; \
	} \
}

typedef struct Interface {
	// Object
	struct Interface__class *_;

	// Interface
	Object *__self__;
} Interface;

typedef struct Interface__class {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct Object__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, Interface *self);
	void (*__dtor__)(Ex **e__, Interface *self);
	uint32 (*hash_code)(Ex **e__, Interface *self); 
	void (*to_string)(Ex **e__, Interface *self, struct String *out);
	// Interface
} Interface__class; 

EXPORT Interface__class Interface__;

#endif // LANG__INTERFACE_H
