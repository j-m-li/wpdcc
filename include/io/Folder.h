/* "$Id: $"
 *
 *                          07 May MMVIII PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#ifndef IO__FOLDER_H
#define IO__FOLDER_H

#include <lang/Object.h>

typedef struct io__Folder {
	// Object
	struct io__Folder__class *_;

	// Folder
	void *handle;
	String *full_path_name;
	utf8 *next_file;
} io__Folder;

typedef struct io__Folder__class {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct Object__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, io__Folder *self);
	void (*__dtor__)(Ex **e__, io__Folder *self);
	uint32 (*hash_code)(Ex **e__, io__Folder *self); 
	void (*to_string)(Ex **e__, io__Folder *self, struct String *out);

	// Folder
	void (*open)(Ex **e__, io__Folder *self, struct String *name,
		struct String *mode);
	void (*close)(Ex **e__, io__Folder *self);
	int8 (*find_next_file)(Ex **e__, io__Folder *self, String *out);
} io__Folder__class; 

EXPORT io__Folder__class io__Folder__;

#endif // IO__FOLDER_H
