/* "$Id: $"
 *
 *                          3 May MMIX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#ifndef LANG__HASH_H
#define LANG__HASH_H

#include <lang/String.h>



typedef struct Hash_data {
	utf8 *hash;
	uint32 hash_code;
	void *data;
	struct Hash_data *next;
} Hash_data;

typedef struct Hash_iterate {
	uint32 index;
	Hash_data *data;
} Hash_iterate;

typedef struct Hash {
	// Object
	struct Hash__class *_;

	// Hash
	Hash_data **data;
	uint32 nb_data;
	uint32 mask;
} Hash;

typedef struct Hash__class {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct Object__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, Hash *self);
	void (*__dtor__)(Ex **e__, Hash *self);
	uint32 (*hash_code)(Ex **e__, Hash *self); 
	void (*to_string)(Ex **e__, Hash *self, String *out);

	// Hash
	Hash_data *(*set)(Ex **e__, Hash *self, String *hash, void *in, void **out);
	void *(*get)(Ex **e__, Hash *self, String *hash);
	uint32 (*get_length)(Ex **e__, Hash *self);
	void (*clear_objects)(Ex **e__, Hash *self);
	void (*unset_first)(Ex **e__, Hash *self, void **out);
	void (*iterate)(Ex **e__, Hash *self, Hash_iterate *state);
} Hash__class; 

EXPORT Hash__class Hash__;


#endif // LANG__HASH_H
