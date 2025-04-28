/* "$Id: $"
 *
 *                          29 July MMIX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#ifndef LANG__ICOMPARABLE_H
#define LANG__ICOMPARABLE_H

#include <lang/Object.h>

typedef struct IComparable {
	// Object
	struct IComparable__class *_;

	// Interface
	struct Object *__self__;
} IComparable;

typedef struct IComparable__class {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct Object__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, IComparable *self);
	void (*__dtor__)(Ex **e__, IComparable *self);
	uint32 (*hash_code)(Ex **e__, IComparable *self); 
	void (*to_string)(Ex **e__, IComparable *self, 
		struct String *out);

	// IComparable
	int8 (*compare_to)(Ex **e__, IComparable *self, 
		struct Object *to);
} IComparable__class; 

#endif // LANG__ICOMPARABLE_H
