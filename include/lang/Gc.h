/* "$Id: $"
 *
 *                          07 May MMVIII PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#ifndef LANG__GC_H
#define LANG__GC_H

#include <lang/Object.h>

typedef struct Gc {
	struct Gc__class *_;
} Gc;

typedef struct Gc__class {
	utf8 *__name__;
	uint32 __size__;
	struct Object__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, Gc *self);
	void (*__dtor__)(Ex **e__, Gc *self);
	uint32 (*hash_code)(Ex **e__, Gc *self); 
	void (*to_string)(Ex **e__, Gc *self, 
		struct String *out);
} Gc__class; 

EXPORT Gc__class Gc__;

EXPORT void *Gc__malloc(Ex **e__, int size);
EXPORT void Gc__free(Ex **e__, void *data);
EXPORT void *Gc__new(Ex **e__, int size);
EXPORT void Gc__delete(Ex **e__, void *data);

#endif // LANG__GC_H
