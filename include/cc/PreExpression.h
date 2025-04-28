/* "$Id: $"
 *
 *                          12 August MMVIII PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#ifndef CC__PREEXPRESSION_H
#define CC__PREEXPRESSION_H

#include <lang/Object.h>
#include <cc/Env.h>

struct cc__PreExpression;

typedef struct cc__PreTree {
	struct cc__PreTree *right;
	struct cc__PreTree *left;
	struct cc__PreTree *parent;
	uint32 type;
	int32 precedence;
	int8 associativity;
	uint32 file;
	uint32 line;
	uint32 char_pos;
	void (*func)(Ex **e__, struct cc__PreExpression *self, 
		struct cc__PreTree *tree);
	int64 value; 
} cc__PreTree;


typedef struct cc__PreExpression {
	// Object
	struct cc__PreExpression__class *_;

	// PreExpression
	cc__PreTree *root;
	cc__PreTree *current;
	cc__Env *env;
} cc__PreExpression;

typedef struct cc__PreExpression__class {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct Object__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, cc__PreExpression *self);
	void (*__dtor__)(Ex **e__, cc__PreExpression *self);
	uint32 (*hash_code)(Ex **e__, cc__PreExpression *self); 
	void (*to_string)(Ex **e__, cc__PreExpression *self, String *out);

	// PreExpression
	void (*push_number)(Ex **e__, cc__PreExpression *self, 
		utf8 *name, uint32 type, uint32 file, uint32 line,
		uint32 char_pos);
	void (*push_punctuator)(Ex **e__, cc__PreExpression *self, 
		uint32 type, uint32 file, uint32 line, uint32 char_pos);
	int8 (*solve)(Ex **e__, cc__PreExpression *self);
	void (*set_env)(Ex **e__, cc__PreExpression *self, cc__Env *env);

} cc__PreExpression__class; 

EXPORT cc__PreExpression__class cc__PreExpression__;


#endif // CC_PREEXPRESSION_H

