/* "$Id: $"
 *
 *                          27 July MMVIII PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#ifndef CC__PREMACROS_H
#define CC__PREMACROS_H

#include <lang/Object.h>
#include <lang/Hash.h>
#include <cc/PreTokens.h>
#include <cc/PreMacro.h>
#include <cc/PreMacroReplaced.h>
#include <cc/Env.h>


typedef struct cc__PreMacros {
	// Object
	struct cc__PreMacros__class *_;

	// PreMacros
	Hash *hash;
	cc__Env *env;
	cc__PreMacroReplaced *replaced;

	cc__PreTokens *line_def;
	cc__PreTokens *file_def;
} cc__PreMacros;

typedef struct cc__PreMacros__class {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct Object__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, cc__PreMacros *self);
	void (*__dtor__)(Ex **e__, cc__PreMacros *self);
	uint32 (*hash_code)(Ex **e__, cc__PreMacros *self); 
	void (*to_string)(Ex **e__, cc__PreMacros *self, String *out);

	// PreMacros
	void (*push_function)(Ex **e__, cc__PreMacros *self, 
		cc__PreToken *identifier, cc__PreTokens *tokens,
		cc__PreToken *arguments, cc__PreToken *start);
	void (*push_object)(Ex **e__, cc__PreMacros *self,
		cc__PreToken *identifier, cc__PreTokens *tokens,
		cc__PreToken *start);
	int8 (*is_defined)(Ex **e__, cc__PreMacros *self, utf8 *identifier);
	void (*undef)(Ex **e__, cc__PreMacros *self, cc__PreToken *id);
	cc__PreMacro *(*get)(Ex **e__, cc__PreMacros *self, utf8 *identifier);
	void (*set_env)(Ex **e__, cc__PreMacros *self, cc__Env *env);
	void (*expand_macros)(Ex **e__, cc__PreMacros *self,
		cc__PreTokens *tokens);
} cc__PreMacros__class; 

EXPORT cc__PreMacros__class cc__PreMacros__;

EXPORT void cc__PreMacros__expand_macros(Ex **e__, cc__PreMacros *self,
		cc__PreTokens *tokens);


#endif // CC__PREMACROS_H

