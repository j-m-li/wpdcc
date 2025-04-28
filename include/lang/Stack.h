/* "$Id: $"
 *
 *                          3 May MMIX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#ifndef LANG__STACK_H
#define LANG__STACK_H

#include <lang/Object.h>

typedef struct Stack {
	// Object
	struct Stack__class *_;

	// Stack
	void **data;
	uint32 nb_data;
	uint32 ma_data;
} Stack;

typedef struct Stack__class {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct Object__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, Stack *self);
	void (*__dtor__)(Ex **e__, Stack *self);
	uint32 (*hash_code)(Ex **e__, Stack *self); 
	void (*to_string)(Ex **e__, Stack *self, String *out);

	// Stack
	void (*push)(Ex **e__, Stack *self, void *in);
	void (*pop)(Ex **e__, Stack *self, void **out);
	uint32 (*get_length)(Ex **e__, Stack *self);
	void (*clear_objects)(Ex **e__, Stack *self);
} Stack__class; 

EXPORT Stack__class Stack__;


#endif // LANG__STACK_H
