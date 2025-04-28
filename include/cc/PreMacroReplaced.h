/* "$Id: $"
 *
 *                          27 July MMVIII PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#ifndef CC__PREMACROREPLACED_H
#define CC__PREMACROREPLACED_H

#include <lang/Object.h>
#include <lang/Array.h>
#include <cc/PreTokens.h>
#include <cc/Env.h>

typedef struct cc__PreMacroReplaced_data {
	utf8 *macro_name;
	int32 *parents_index;
	int32 nb_parents;
} cc__PreMacroReplaced_data;

typedef struct cc__PreMacroReplaced {
	// Object
	struct cc__PreMacroReplaced__class *_;

	// PreMacroReplaced
	cc__Env *env;
	cc__PreMacroReplaced_data *replaced;
	int32 ma_replaced;
	int32 nb_replaced;
} cc__PreMacroReplaced;

typedef struct cc__PreMacroReplaced__class {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct Object__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, cc__PreMacroReplaced *self);
	void (*__dtor__)(Ex **e__, cc__PreMacroReplaced *self);
	uint32 (*hash_code)(Ex **e__, cc__PreMacroReplaced *self); 
	void (*to_string)(Ex **e__, cc__PreMacroReplaced *self, String *out);

	// PreMacroReplaced
	int32 (*push)(Ex **e__, cc__PreMacroReplaced *self, utf8 *id,
		int32 parent);
	void (*add)(Ex **e__, cc__PreMacroReplaced *self, int32 index, 
		int32 parent);
	int8 (*is_already_replaced)(Ex **e__, cc__PreMacroReplaced *self,
		cc__PreToken *tk);
	void (*clean)(Ex **e__, cc__PreMacroReplaced *self);
} cc__PreMacroReplaced__class; 

EXPORT cc__PreMacroReplaced__class cc__PreMacroReplaced__;


#endif // CC__PREMACROREPLACED_H

