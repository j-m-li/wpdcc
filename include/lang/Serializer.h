/* "$Id: $"
 *
 *                          07 May 2008 PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#ifndef LANG__SERIALIZER_H
#define LANG__SERIALIZER_H

#include <lang/Object.h>

typedef struct Serializer__struct {
	struct Serializer__class_struct *_;
} Serializer;

typedef struct Serializer__class_struct {
	utf8 *__name__;
	uint32 __size__;
	struct Object__class_struct *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, Serializer *self);
	void (*dispose)(Ex **e__, Serializer *self);
	int8 (*compare)(Ex **e__, Serializer *self, Object *in);
	uint32 (*hash_code)(Ex **e__, Serializer *self); 
	void (*to_string)(Ex **e__, Serializer *self,
		struct String__struct *out);
	void (*serialize)(Ex **e__, Serializer *self, Serializer *out);
	void (*unserialize)(Ex **e__, Serializer *self, Serializer *in);
} Serializer__class; 

EXPORT Serializer__class Serializer__;

#endif // LANG__SERIALIZER_H
