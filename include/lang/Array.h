/* "$Id: $"
 *
 *                          3 May MMIX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#ifndef LANG__ARRAY_H
#define LANG__ARRAY_H

#include <lang/Object.h>

typedef struct Array {
	// Object
	struct Array__class *_;

	// Array
	void **data;
	uint32 nb_data;
	uint32 ma_data;
} Array;

typedef struct Array__class {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct Object__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, Array *self);
	void (*__dtor__)(Ex **e__, Array *self);
	uint32 (*hash_code)(Ex **e__, Array *self); 
	void (*to_string)(Ex **e__, Array *self, String *out);

	// Array
	void (*set)(Ex **e__, Array *self, uint32 index, void *in);
	void *(*get)(Ex **e__, Array *self, uint32 index);
	uint32 (*get_length)(Ex **e__, Array *self);
	void (*clear_objects)(Ex **e__, Array *self);
} Array__class; 

EXPORT Array__class Array__;


#endif // LANG__ARRAY_H
