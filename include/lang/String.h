/* "$Id: $"
 *
 *                          07 May MMVIII PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <lang/Object.h>

#ifndef LANG__STRING_H
#define LANG__STRING_H

#define STR(s, t) { \
	(s)._ = &String_auto__; \
	(s).size = 0; \
	(s).cstr = (utf8*)t; \
	(s).line = __LINE__; \
	(s).file = (utf8*)__FILE__; \
}

#define STRN(s, t, n) { \
	(s)._ = &String__; \
	(s).size = n; \
	(s).cstr = (utf8*)t; \
	(s).line = __LINE__; \
	(s).file = (utf8*)__FILE__; \
}

typedef struct String {
	// Object
	struct String__class *_;

	// String
	int32 size;
	utf8 *cstr;

	//String_auto
	int32 line;
	utf8 *file;	
} String;

typedef struct String__class {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct Object__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, String *self);
	void (*__dtor__)(Ex **e__, String *self);
	uint32 (*hash_code)(Ex **e__, String *self); 
	void (*to_string)(Ex **e__, String *self, String *out);

	// String
	void (*io__i_serializable)(Ex **e__, String *self,
		struct io__ISerializable *out);
	void (*serialize)(Ex **e__, struct io__ISerializable *itf, 
		struct io__Serializer *in);
	void (*unserialize)(Ex **e__, struct io__ISerializable *itf,
		struct io__Serializer *in);
	void (*i_comparable)(Ex **e__, String *self,
		struct IComparable *out);
	int8 (*compare_to)(Ex **e__, 
		struct IComparable *itf, Object *in);
	void (*add)(Ex **e__, String *self, String *in);
	void (*set)(Ex **e__, String *self, String *in);
	void (*clear)(Ex **e__, String *self);
	utf8 *(*get_cstr)(Ex **e__, String *self);
	int32 (*get_size)(Ex **e__, String *self);
	void (*printf)(Ex **e__, String *self, String *format, ...);
	int32 (*index_of)(Ex **e__, String *self, String *needle);
} String__class; 

EXPORT void String__add(Ex **e__, String *self, String *in);
EXPORT int32 String__utf2ucs(const utf8 *buf, int32 len, 
	int32 *ucs);
EXPORT int32 String__nonspacing(int32 ucs);

EXPORT String__class String__;
EXPORT String__class String_auto__;

#endif // LANG__STRING_H
