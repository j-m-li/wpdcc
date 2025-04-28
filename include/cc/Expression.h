/* "$Id: $"
 *
 *                          11 Jully MMX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#ifndef CC__EXPRESSION_H
#define CC__EXPRESSION_H

#include <cc/PreToken.h>
#include <cc/Type.h>

struct cc__Expression;

typedef struct cc__ExpressionTree {
	struct cc__ExpressionTree *right;
	struct cc__ExpressionTree *left;
	struct cc__ExpressionTree *parent;
	struct cc__ExpressionTree *next;
	cc__Type *type;
	cc__PreToken *token;
} cc__ExpressionTree;

typedef struct cc__Expression {
	// Object
	struct cc__Expression__class *_;

	// Expression
	cc__ExpressionTree *root;
	cc__ExpressionTree *current;
	struct cc__Env *env;
} cc__Expression;

typedef struct cc__Expression__class {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct Object__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, cc__Expression *self);
	void (*__dtor__)(Ex **e__, cc__Expression *self);
	uint32 (*hash_code)(Ex **e__, cc__Expression *self); 
	void (*to_string)(Ex **e__, cc__Expression *self, String *out);

	// Expression
	void (*push)(Ex **e__, cc__Expression *self, 
		cc__Type *type, cc__PreToken *token);
	void (*finish)(Ex **e__, cc__Expression *self);

	void (*auto_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*break_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*case_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*char_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*const_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*continue_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*default_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*do_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*double_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*else_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*enum_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*extern_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*float_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*for_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*goto_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*if_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*inline_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*int_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*long_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*register_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*restrict_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*return_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*short_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*signed_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*static_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*struct_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*switch_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*typedef_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*union_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*unsigned_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*void_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*volatile_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*while_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*_Bool_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*_Complex_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*_Imaginary_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (* /**/pointer_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (* /**/object_type_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (* /**/function_type_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (* /**/label_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (* /**/typedef_identifier_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (* /**/string_constant_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (* /**/character_constant_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (* /**/number_constant_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*paranthes_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*plusplus_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*minusminus_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*and_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*star_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*plus_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*minus_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*colon_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*semicolon_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*bracket_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*brace_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*closing_brace_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*closing_paranthesis_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*closing_bracket_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*array_subscripting_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*function_calls_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*structure_member_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*pointer_structure_member_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*postfix_increment_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*postfix_decrement_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*compound_literals_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*prefix_increment_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*prefix_decrement_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*address_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*indirection_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*unary_plus_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*unary_minus_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*bitwise_complement_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*logical_negation_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*sizeof_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*cast_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*multiply_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*divide_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*modulo_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*addition_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*substraction_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*bitwise_shift_left_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*bitwise_shift_right_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*less_than_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*greater_than_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*less_than_or_equal_to_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*greater_than_or_equal_to_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*equal_to_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*not_equal_to_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*bitwise_and_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*bitwise_exclusive_or_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*bitwise_inclusive_or_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*logical_and_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*logical_or_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*conditonal_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*simple_assignment_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*multiply_assignment_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*divide_assignment_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*modulo_assignment_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*addition_assignment_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*substraction_assignment_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*shift_left_assignment_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*shift_right_assignment_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*and_assignment_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*exclusive_or_assignment_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*inclusive_or_assignment_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
	void (*comma_)(Ex **e__,
		cc__Expression *self, cc__ExpressionTree *tk);
} cc__Expression__class; 

EXPORT cc__Expression__class cc__Expression__;


#endif // CC__EXPRESSION_H

