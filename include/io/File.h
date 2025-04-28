/* "$Id: $"
 *
 *                          07 May MMVIII PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#ifndef IO__FILE_H
#define IO__FILE_H

#include <lang/String.h>

#define echo(s) {io__File__stdout->_->add(e__, io__File__stdout, s);}

#define ECHO(s) {String tmp; STR(tmp, s); echo(&tmp);}

#define ECHO_NUM(s) {String tmp; \
	STR(tmp, (utf8*)"%d"); \
	io__File__stdout->_->printf( \
		e__, io__File__stdout, &tmp, s);}

typedef struct io__File {
	// Object
	struct io__File__class *_;

	// String
	int32 size;
	utf8 *cstr;	

	// File
	void *handle;
	struct String *full_path_name;
	int32 line;
	int32 alloced;
	int32 read_len;
	int8 eof;
} io__File;

typedef struct io__File__class {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct String__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, io__File *self);
	void (*__dtor__)(Ex **e__, io__File *self);
	uint32 (*hash_code)(Ex **e__, io__File *self); 
	void (*to_string)(Ex **e__, io__File *self, struct String *out);

	// String
	void (*io__i_serializable)(Ex **e__, io__File *self,
		struct io__ISerializable *out);
	void (*serialize)(Ex **e__, io__ISerializable *itf,
		io__Serializer *out);
	void (*unserialize)(Ex **e__, io__ISerializable *itf,
		io__Serializer *out);
	void (*i_comparable)(Ex **e__, io__File *self,
		struct IComparable *out);
	int8 (*compare_to)(Ex **e__, IComparable *itf, Object *in);
	void (*add)(Ex **e__, io__File *self, struct String *in);
	void (*set)(Ex **e__, io__File *self, struct String *in);
	void (*clear)(Ex **e__, io__File *self);
	utf8 *(*get_cstr)(Ex **e__, io__File *self);
	int32 (*get_size)(Ex **e__, io__File *self);
	void (*printf)(Ex **e__, io__File *self, 
		struct String *format, ...);
	int32 (*index_of)(Ex **e__, io__File *self,
		struct String *needle);

	// File
	void (*open)(Ex **e__, io__File *self, struct String *name,
		struct String *mode);
	void (*close)(Ex **e__, io__File *self);
	void (*seek)(Ex **e__, io__File *self, int64 pos);
	void (*read_line)(Ex **e__, io__File *self, struct String *out);
	int8 (*end_of_file)(Ex **e__, io__File *self);
	int32 (*get_line_number)(Ex **e__, io__File *self);
} io__File__class; 

EXPORT io__File__class io__File__;

EXPORT io__File *io__File__stdin;
EXPORT io__File *io__File__stdout;
EXPORT io__File *io__File__stderr;

#endif // IO__FILE_H
