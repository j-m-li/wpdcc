/* "$Id: $"
 *
 *                          07 May 2008 PUBLIC DOMAIN
 *             The author disclaims copyright to self source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#ifndef IO__SERIAL_H
#define IO__SERIAL_H

#include <io/File.h>

typedef struct io__Serial__struct {
	// Object
	struct io__Serial__class_struct *_;
	__REF__();

	// String
	int32 size;
	utf8 *cstr;	

	// File
	void *handle;
	struct String__struct *full_path_name;
	int32 line;
	int32 alloced;
	int32 read_len;
	int8 eof;
} io__Serial;

typedef struct io__Serial__class_struct {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct String__class_struct *super;
	void (*__init__)(void);
	void (*__ctor__)(io__Serial *self);
	void (*dispose)(io__Serial *self);
	uint8 (*compare)(io__Serial *self, Object *in);
	uint32 (*hash_code)(io__Serial *self); 
	void (*to_string)(io__Serial *self, struct String__struct *out);
	void (*serialize)(io__Serial *self, struct Serializer__struct *out);
	void (*unserialize)(io__Serial *self, struct Serializer__struct *in);

	// String
	void (*add)(io__Serial *self, struct String__struct *in);
	void (*set)(io__Serial *self, struct String__struct *in);
	void (*clear)(io__Serial *self);
	utf8 *(*get_cstr)(io__Serial *self);
	int32 (*get_size)(io__Serial *self);
	void (*printf)(io__Serial *self, struct String__struct *format, ...);
	int32 (*index_of)(io__Serial *self, struct String__struct *needle);

	// File
	void (*open)(io__Serial *self, struct String__struct *name,
		struct String__struct *mode);
	void (*close)(io__Serial *self);
	void (*seek)(io__Serial *self, int64 pos);
	void (*read_line)(io__Serial *self, struct String__struct *out);
	int8 (*end_of_file)(io__Serial *self);
	int32 (*get_line_number)(io__Serial *self);
} io__Serial__class; 

EXPORT io__Serial__class io__Serial__;

#endif // IO__SERIAL_H
