/* "$Id: $"
 *
 *                          07 May MMVIII PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#ifndef IO__SERIALIZER_H
#define IO__SERIALIZER_H

#include <lang/Object.h>

typedef struct io__Serializer {
	// Object
	struct io__Serializer__class *_;
} io__Serializer;

typedef struct io__Serializer__class {
	utf8 *__name__;
	uint32 __size__;
	struct Object__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, io__Serializer *self);
	void (*__dtor__)(Ex **e__, io__Serializer *self);
	uint32 (*hash_code)(Ex **e__, io__Serializer *self); 
	void (*to_string)(Ex **e__, io__Serializer *self,
		struct String *out);
} io__Serializer__class; 

EXPORT io__Serializer__class io__Serializer__;

#endif // IO__SERIALIZER_H
