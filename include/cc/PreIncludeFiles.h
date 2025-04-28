/* "$Id: $"
 *
 *                          24 july MMIX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#ifndef CC__PREINCLUDEFILES_H
#define CC__PREINCLUDEFILES_H

#include <lang/Object.h>
#include <lang/Array.h>
#include <lang/Buffer.h>
#include <lang/Stack.h>
#include <cc/Env.h>

typedef struct cc__PreIncludeFiles_tree {
	uint32 file;
	uint32 parent_line;
	uint32 parent;
	uint32 depth;
	int8 *condition_stack;
	int32 nb_condition;
	int32 alloced_condition;
} cc__PreIncludeFiles_tree;

typedef struct cc__PreIncludeFiles {
	// Object
	struct cc__PreIncludeFiles__class *_;

	// IncludeFiles
	uint32	current_file_id;
	Array *files_name;
	Stack *files;

	cc__PreIncludeFiles_tree *include_tree;
	uint32 nb_tree;
	uint32 ma_tree;

	Array *include_var;
	Array *once;

	struct cc__Env *env;

	int8 *condition_stack;
	int32 nb_condition;
	int32 alloced_condition;
	int8 condition;

} cc__PreIncludeFiles;

typedef struct cc__PreIncludeFiles__class {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct Object__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, cc__PreIncludeFiles *self);
	void (*__dtor__)(Ex **e__, cc__PreIncludeFiles *self);
	uint32 (*hash_code)(Ex **e__, cc__PreIncludeFiles *self); 
	void (*to_string)(Ex **e__, cc__PreIncludeFiles *self, String *out);

	// IncludeFiles
	void (*clear)(Ex **e__, cc__PreIncludeFiles *self);
	void (*set_current_file)(Ex **e__, cc__PreIncludeFiles *self,
		String *name, uint32 parent_line);
	String *(*get_current_file_full_path)(Ex **e__,
		cc__PreIncludeFiles *self);
	void (*revert_file)(Ex **e__, cc__PreIncludeFiles *self);
	void (*get_include_file)(Ex **e__, cc__PreIncludeFiles *self,
		String *name, String *out, String *current);
	String *(*get_file_path)(Ex **e__, cc__PreIncludeFiles *self,
		uint32 file);
	void (*init_include_var)(Ex **e__, cc__PreIncludeFiles *self,
		struct cc__Env *env);
	void (*get_line)(Ex **e__, cc__PreIncludeFiles *self, String *s,
		uint32 file, uint32 line, uint32 char_pos);
	void (*get_include_tree)(Ex **e__, cc__PreIncludeFiles *self, String *s,
		uint32 file, uint32 line);
	void (*get_previous_def)(Ex **e__, cc__PreIncludeFiles *self, String *s,
		uint32 file, uint32 line);
	void (*add_include_path)(Ex **e__, cc__PreIncludeFiles *self, 
		String *path);
	io__File *(*open_file)(Ex **e__, cc__PreIncludeFiles *self,
		String *name);
	void (*include_once)(Ex **e__, cc__PreIncludeFiles *self);
	void (*push_condition)(Ex **e__, cc__PreIncludeFiles *self, int8 value);
	void (*pop_condition)(Ex **e__, cc__PreIncludeFiles *self);
} cc__PreIncludeFiles__class; 

EXPORT cc__PreIncludeFiles__class cc__PreIncludeFiles__;


#endif // CC__PREINCLUDEFILES_H

