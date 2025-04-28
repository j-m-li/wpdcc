/* "$Id: $"
 *
 *                          27 July MMVIII PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#ifndef CC__PREMACRO_H
#define CC__PREMACRO_H

#include <lang/Object.h>
#include <lang/Hash.h>
#include <cc/PreTokens.h>
#include <cc/Env.h>

typedef struct cc__PreMacro {
	// Object
	struct cc__PreMacro__class *_;

	// PreMacro
	cc__PreTokens *body;
	utf8 **parameters;
	int32 nb_parameters;
	int8 is_object;
	uint32 file;
	uint32 line;
	uint32 char_pos;
} cc__PreMacro;

typedef struct cc__PreMacro__class {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct Object__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, cc__PreMacro *self);
	void (*__dtor__)(Ex **e__, cc__PreMacro *self);
	uint32 (*hash_code)(Ex **e__, cc__PreMacro *self); 
	void (*to_string)(Ex **e__, cc__PreMacro *self, String *out);

	// PreMacro
	void (*set)(Ex **e__, cc__PreMacro *self, cc__PreTokens *body,
		utf8 **parameters, int32 nb_parameters, int8 is_object,
		uint32 file, uint32 line, uint32 char_pos);

} cc__PreMacro__class; 

EXPORT cc__PreMacro__class cc__PreMacro__;


#endif // CC__PREMACRO_H

