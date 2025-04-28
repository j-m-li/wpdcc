/* "$Id: $"
 *
 *                          10 October MMIX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#ifndef CC__TYPE_H
#define CC__TYPE_H

#include <lang/Object.h>
#include <lang/Hash.h>
#include <cc/PreToken.h>

#include "Type_generated.h"

typedef struct cc__Type_data {
	struct cc__Type **sub;
	int32 nb_sub;
	struct cc__Type **rett;
	int32 nb_rett;
	struct cc__Type **param;
	int32 nb_param;
	void *init_value;
	int32 init_value_size;
} cc__Type_data;

struct cc__Expression_struct;
struct cc__ExpressionTree_struct;

typedef struct cc__Type {
	// Object
	struct cc__Type__class *_;

	// Type
	uint32 id;
	uint32 flags;
	cc__PreToken *name;
	cc__Type_data *data;
	uint16 precedence;
	uint16 associativity;
	void (*to_basic_block)(Ex **, struct cc__Expression_struct *, 
		struct cc__ExpressionTree_struct *);
} cc__Type;

typedef struct cc__Type__class {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct Object__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, cc__Type *self);
	void (*__dtor__)(Ex **e__, cc__Type *self);
	uint32 (*hash_code)(Ex **e__, cc__Type *self); 
	void (*to_string)(Ex **e__, cc__Type *self, String *out);

	// Type
	void (*init)(Ex **e__, cc__Type *self, uint32 id, cc__PreToken *name);
	void (*add_sub_type)(Ex **e__, cc__Type *self, cc__Type *sub);
	void (*add_parameter)(Ex **e__, cc__Type *self, cc__Type *param);
	void (*add_return_type)(Ex **e__, cc__Type *self, cc__Type *rett);
} cc__Type__class; 

EXPORT cc__Type__class cc__Type__;


#endif // CC__TYPE_H

