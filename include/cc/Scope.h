/* "$Id: $"
 *
 *                          10 October MMIX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#ifndef CC__SCOPE_H
#define CC__SCOPE_H

#include <lang/Object.h>
#include <lang/Hash.h>
#include <cc/Type.h>

typedef struct cc__Scope {
	// Object
	struct cc__Scope__class *_;

	// Scope
	struct cc__Scope *parent;
	Hash *objects;
	cc__Type *current_object;
	uint32 bracket;
	uint32 brace;
	uint32 paranthesis;
	uint32 last_rett_id;
} cc__Scope;

typedef struct cc__Scope__class {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct Object__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, cc__Scope *self);
	void (*__dtor__)(Ex **e__, cc__Scope *self);
	uint32 (*hash_code)(Ex **e__, cc__Scope *self); 
	void (*to_string)(Ex **e__, cc__Scope *self, String *out);

	// Scope
	cc__Type *(*get_type_of)(Ex **e__, cc__Scope *self, String *name);
	cc__Type *(*new_object)(Ex **e__, cc__Scope *self);
	void (*name_object)(Ex **e__, cc__Scope *self, cc__PreToken *name);
	void (*end_object)(Ex **e__, cc__Scope *self, cc__Type **old);
	void (*end_parameter)(Ex **e__, cc__Scope *self, cc__Type **old);
	void (*add_return_type)(Ex **e__, cc__Scope *self, cc__Type *type);
	void (*set_type_id)(Ex **e__, cc__Scope *self, uint32 id);
	void (*end_of_function)(Ex **e__, cc__Scope *self);
	cc__Scope *(*new_scope)(Ex **e__, cc__Scope *self);
	cc__Scope *(*end_scope)(Ex **e__, cc__Scope *self);
	void (*dump)(Ex **e__, cc__Scope *self);
} cc__Scope__class;

EXPORT cc__Scope__class cc__Scope__;


#endif // CC__SCOPE_H

