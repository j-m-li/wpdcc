/* "$Id: $"
 *
 *                          29 July MMIX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#ifndef IO__ISERIALIZABLE_H
#define IO__ISERIALIZABLE_H

#include <lang/Object.h>

typedef struct io__ISerializable {
	// Object
	struct io__ISerializable__class *_;

	// Interface
	struct Object *__self__;
} io__ISerializable;

typedef struct io__ISerializable__class {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct Object__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, io__ISerializable *self);
	void (*__dtor__)(Ex **e__, io__ISerializable *self);
	uint32 (*hash_code)(Ex **e__, io__ISerializable *self); 
	void (*to_string)(Ex **e__, io__ISerializable *self,
		struct String *out);

	// io__ISerializable
	void (*serialize)(Ex **e__, io__ISerializable *self, 
		struct io__Serializer *out);
	void (*unserialize)(Ex **e__, io__ISerializable *self, 
		struct io__Serializer *in);
} io__ISerializable__class; 

#endif // IO__ISERIALIZABLE_H
