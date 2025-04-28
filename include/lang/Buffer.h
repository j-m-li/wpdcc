/* "$Id: $"
 *
 *                          21 July MMIX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#ifndef LANG__BUFFER_H
#define LANG__BUFFER_H

#include <lang/String.h>

typedef struct Buffer {
	// Object
	struct Buffer__class *_;

	// String
	int32 size;
	utf8 *cstr;	

	// Buffer
	int32 alloced;
} Buffer;

typedef struct Buffer__class {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct String__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, Buffer *self);
	void (*__dtor__)(Ex **e__, Buffer *self);
	uint32 (*hash_code)(Ex **e__, Buffer *self); 
	void (*to_string)(Ex **e__, Buffer *self, struct String *out);

	// String
	void (*io__i_serializable)(Ex **e__, Buffer *self,
		io__ISerializable *out);
	void (*serialize)(Ex **e__, io__ISerializable *itf,
		io__Serializer *out);
	void (*unserialize)(Ex **e__, io__ISerializable *itf,
		io__Serializer *out);
	void (*i_comparable)(Ex **e__, Buffer *self, IComparable *out);
	int8 (*compare_to)(Ex **e__, IComparable *itf, Object *in);
	void (*add)(Ex **e__, Buffer *self, struct String *in);
	void (*set)(Ex **e__, Buffer *self, struct String *in);
	void (*clear)(Ex **e__, Buffer *self);
	utf8 *(*get_cstr)(Ex **e__, Buffer *self);
	int32 (*get_size)(Ex **e__, Buffer *self);
	void (*printf)(Ex **e__, Buffer *self, 
		struct String *format, ...);
	int32 (*index_of)(Ex **e__, Buffer *self,
		struct String *needle);

	// Buffer
	void (*add_data)(Ex **e__, Buffer *self, utf8 *data, int32 size);
} Buffer__class; 

EXPORT Buffer__class Buffer__;

EXPORT void Buffer__add(Ex **e__, Buffer *self, struct String *in);
EXPORT void Buffer__add_data(Ex **e__, Buffer *self, utf8 *data, int32 size);

#endif // LANG__BUFFER_H
