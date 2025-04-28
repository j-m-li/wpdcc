/* "$Id: $"
 *
 *                          9 november MMVIII PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#ifndef CC__ENV_H
#define CC__ENV_H

#include <lang/Object.h>
#include <lang/Array.h>
#include <lang/Buffer.h>
#include <cc/PreIncludeFiles.h>
#include <cc/Parser.h>

#define CC__WARN(txt, file, line, char_pos) { \
	String s1; \
	STR(s1, txt); \
	self->env->_->log(e__, self->env, &s1, file, line, char_pos); \
}

#define CC__WARN_DYN(txt, file, line, char_pos) { \
	self->env->_->log(e__, self->env, txt, file, line, char_pos); \
}

struct cc__PreProcessor;

typedef struct cc__Env {
	// Object
	struct cc__Env__class *_;

	// Env
	cc__PreIncludeFiles *include;

	uint32 last_ifelif_line;
	uint32 last_ifelif_char;

	Array *lib_var;
	Array *sources;
	Array *define_var;

	struct cc__PreProcessor *preprocessor;
	int8 only_preprocess;
	int8 add_line_directives;
	io__File *out;
	Buffer *buffer;
	uint32 last_file;
	uint32 last_line;

	int8 pragma_stdc_fp_contract;
	int8 pragma_stdc_fenv_access;
	int8 pragma_stdc_cx_limited_range;

	cc__Parser *parser;
} cc__Env;

typedef struct cc__Env__class {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct Object__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, cc__Env *self);
	void (*__dtor__)(Ex **e__, cc__Env *self);
	uint32 (*hash_code)(Ex **e__, cc__Env *self); 
	void (*to_string)(Ex **e__, cc__Env *self, String *out);

	// Env
	void (*init)(Ex **e__, cc__Env *self);
	void (*log)(Ex **e__, cc__Env *self, String *s1, uint32 file, 
		uint32 line, uint32 char_pos);
	void (*run)(Ex **e__, cc__Env *self);
	void (*parse)(Ex **e__, cc__Env *self);
	uint32 (*get_type_of_cstr)(Ex **e__, cc__Env *self,
		utf8 *cstr, int32 size);
} cc__Env__class; 

EXPORT cc__Env__class cc__Env__;


#endif // CC__ENV_H

