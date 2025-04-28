/* "$Id: $"
 *
 *                          11 Jully MMX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <cc/Expression.h>
#include <cc/Env.h>
#include <cc/Parser.h>
#include <cc/Type.h>

static void syntax_error(Ex **e__, cc__Expression *self, cc__PreToken *token);

static void __init__(Ex **e__)
{
	(void)e__;
	cc__Expression__.super = &Object__;
	return;
}

static cc__ExpressionTree *new_tree(Ex **e__)
{
	F("new_tree");
	cc__ExpressionTree *tk;
	
	tk = (cc__ExpressionTree*)Gc__malloc(e__, sizeof(cc__ExpressionTree));
	CHECK(error);
	tk->parent = 0;
	tk->left = 0;
	tk->right = 0;
	tk->type = 0;
	tk->token = 0;	
	tk->next = 0;
	return tk;
error:
	return 0;
}

static void __ctor__(Ex **e__, cc__Expression *self)
{
	F("__ctor__");

	self->root = new_tree(e__);
	CHECK(error);
	self->current = self->root;
	self->env = 0;
error:
	return;
}


static void destroy(Ex **e__, cc__ExpressionTree *t) 
{
	F("destroy");

	if (t == 0) return;

	destroy(e__, t->next);
	CHECK(error1);
error1:
	if (t->token != 0) delete_(t->token);
	CHECK(error3);
error3:
	Gc__free(e__, t);
	CHECK(error4);
error4:
	return;
}

static void __dtor__(Ex **e__, cc__Expression *self)
{
	F("__dtor__");

	destroy(e__, self->root);
	CHECK(error1);
error1:
	return;
}

static uint32 hash_code(Ex **e__, cc__Expression *self)
{
	(void)e__;
#ifdef _MSC_VER
	return *((uint32*)&self) >> 4;
#else 
	return (uint32)self >> 4;
#endif
} 

static void to_string(Ex **e__, cc__Expression *self, String *out)
{
	F("to_string");
	String s;

	STR(s, self->_->__name__);
	out->_->add(e__, out, &s);
	CHECK(error);
error:
	return;
}

static void push(Ex **e__, cc__Expression *self,
	cc__Type *type, cc__PreToken *token)
{
	F("push");
	cc__ExpressionTree *tk;

	tk = new_tree(e__);
	CHECK(error1);
	tk->token = token;
	tk->type = type;
	self->current->next = tk;
	self->current = tk;
	return;
//error2:
//	destroy(e__, tk);
error1:
	return;
}


static void find_type(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("find_type");

}

static void place_in_tree(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("place_in_tree");

	if (tk->type->precedence >= 300 && tk->type->precedence <= 400) {
		find_type(e__, self, tk);
		CHECK(error1);
	}

	if (self->current->parent != 0) {
		while (self->current->type->precedence > tk->type->precedence) {
			self->current = self->current->parent;
		}
		if (self->current->type->precedence == tk->type->precedence &&
			self->current->type->associativity != 
				CC__TOKEN_ASSOCIATIVITY_RIGHT)
		{
			self->current = self->current->parent;
		}
	}
	if (self->current->left == 0) {
		self->current->left = tk;
		tk->parent = self->current;
		self->current = tk;
	} else if (self->current->right == 0) {
		self->current->right = tk;
		tk->parent = self->current;
		self->current = tk;
	} else {
		syntax_error(e__, self, tk->token);
		CHECK(error1);
	}
	return;
error1:
	return;
}

static void left_is_pointer_to_object(
	Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
}


static void left_is_pointer_to_function(
	Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
}

static void right_arguments_agree_with_parameters(
	Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
}

static void structure_member_constraint(
	Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
}

static void pointer_structure_member_constraint(
	Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
}

static void left_is_qualified_or_unqualified_real_or_pointer(
	Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
}

static void right_is_modifiable_lvalue(
	Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
}

static void right_is_qualified_or_unqualified_real_or_pointer(
	Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
}

static void left_is_modifiable_lvalue(
	Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
}

static void compound_literals_constraint(
	Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
}


static void type_is_void_or_qualified_or_unqualified_scalar(
	Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
}

static void right_is_pointer(
	Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
}

static void address_constraint(
	Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
}

static void right_is_arithmetic(
	Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
}

static void left_is_arithmetic(
	Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
}

static void right_is_integer(
	Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
}

static void left_is_integer(
	Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
}

static void right_is_scalar(
	Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
}

static void left_is_scalar(
	Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
}

static void both_are_scalar(
	Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
}


static void sizeof_constraint(
	Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
}

static void addition_constraint(
	Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
}

static void substraction_constraint(
	Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
}

static void both_are_integer(
	Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
}

static void less_greater_constraint(
	Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
}

static void equal_constraint(
	Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
}


static void right_is_conditional2(
	Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
}

static void both_are_valid_conditional2_operand(
	Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
}

static void simple_assignment_constraint(
	Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
}

static void addition_assignment_constraint(
	Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
}

/* ******************************************************************** */
static void auto_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("auto_");
	return;
error1:
	return;
};
static void break_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("break_");
	return;
error1:
	return;
};
static void case_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("case_");
	return;
error1:
	return;
};
static void char_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("char_");
	return;
error1:
	return;
};
static void const_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("const_");
	return;
error1:
	return;
};
static void continue_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("continue_");
	return;
error1:
	return;
};
static void default_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("default_");
	return;
error1:
	return;
};
static void do_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("do_");
	return;
error1:
	return;
};
static void double_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("double_");
	return;
error1:
	return;
};
static void else_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("else_");
	return;
error1:
	return;
};
static void enum_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("enum_");
	return;
error1:
	return;
};
static void extern_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("extern_");
	return;
error1:
	return;
};
static void float_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("float_");
	return;
error1:
	return;
};
static void for_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("for_");
	return;
error1:
	return;
};
static void goto_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("goto_");
	return;
error1:
	return;
};
static void if_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("if_");
	return;
error1:
	return;
};
static void inline_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("inline_");
	return;
error1:
	return;
};
static void int_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("int_");
	return;
error1:
	return;
};
static void long_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("long_");
	return;
error1:
	return;
};
static void register_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("register_");
	return;
error1:
	return;
};
static void restrict_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("restrict_");
	return;
error1:
	return;
};
static void return_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("return_");
	return;
error1:
	return;
};
static void short_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("short_");
	return;
error1:
	return;
};
static void signed_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("signed_");
	return;
error1:
	return;
};
static void static_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("static_");
	return;
error1:
	return;
};
static void struct_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("struct_");
	return;
error1:
	return;
};
static void switch_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("switch_");
	return;
error1:
	return;
};
static void typedef_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("typedef_");
	return;
error1:
	return;
};
static void union_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("union_");
	return;
error1:
	return;
};
static void unsigned_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("unsigned_");
	return;
error1:
	return;
};
static void void_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("void_");
	return;
error1:
	return;
};
static void volatile_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("volatile_");
	return;
error1:
	return;
};
static void while_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("while_");
	return;
error1:
	return;
};
static void _Bool_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("_Bool_");
	return;
error1:
	return;
};
static void _Complex_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("_Complex_");
	return;
error1:
	return;
};
static void _Imaginary_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("_Imaginary_");
	return;
error1:
	return;
};
static void /**/pointer_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("/**/pointer_");
	return;
error1:
	return;
};
static void /**/object_type_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("/**/object_type_");
	return;
error1:
	return;
};
static void /**/function_type_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("/**/function_type_");
	return;
error1:
	return;
};
static void /**/label_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("/**/label_");
	return;
error1:
	return;
};
static void /**/typedef_identifier_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("/**/typedef_identifier_");
	return;
error1:
	return;
};
static void /**/string_constant_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("/**/string_constant_");
	return;
error1:
	return;
};
static void /**/character_constant_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("/**/character_constant_");
	return;
error1:
	return;
};

static uint32 get_dec_number(utf8 *in, uint64 *out)
{
	uint32 pos = 0;
	while (*in) {
		if (*in >= '0' && *in <= '9') {
			*out *= 10;
			*out += *in - '0';
		} else {
			return pos;
		}
		in++;
		pos++;
	}
	return pos;
}

static void /**/number_constant_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("/**/number_constant_");
	uint64 number = 0;
	uint32 pos;
	uint32 tid[3];
	cc__Type *t;
	
	pos = get_dec_number(tk->token->identifier, &number);
	if (number <= 0xFF) {
		tid[0] = CC__TOKEN_NUMBER_CONSTANT;
		tid[1] = CC__TOKEN_UNSIGNED;
		tid[2] = CC__TOKEN_CHAR;
		t = self->env->parser->_->get_composite_type(e__,
			self->env->parser, tid, 3);
	} else if (number <= 0xFFFF) {
	} else if (number <= 0xFFFFFFFF) {
	} else {
	}
	syntax_error(e__, self, tk->token);
	CHECK(error1);

	return;
error1:
	return;
};

static void paranthes_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("paranthes_");
	return;
error1:
	return;
};
static void plusplus_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("plusplus_");
	return;
error1:
	return;
};
static void minusminus_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("minusminus_");
	return;
error1:
	return;
};
static void and_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("and_");
	return;
error1:
	return;
};
static void star_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("star_");
	return;
error1:
	return;
};
static void plus_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("plus_");
	return;
error1:
	return;
};
static void minus_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("minus_");
	return;
error1:
	return;
};
static void colon_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("colon_");
	return;
error1:
	return;
};
static void semicolon_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("semicolon_");
	return;
error1:
	return;
};
static void bracket_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("bracket_");
	return;
error1:
	return;
};
static void brace_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("brace_");
	return;
error1:
	return;
};
static void closing_brace_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("closing_brace_");
	return;
error1:
	return;
};
static void closing_paranthesis_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("closing_paranthesis_");
	return;
error1:
	return;
};
static void closing_bracket_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("closing_bracket_");
	return;
error1:
	return;
};
static void array_subscripting_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("array_subscripting_");
	left_is_pointer_to_object(e__, self, tk);
	CHECK(error1);
	right_is_integer(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void function_calls_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("function_calls_");
	left_is_pointer_to_function(e__, self, tk);
	CHECK(error1);
	right_arguments_agree_with_parameters(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void structure_member_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("structure_member_");
	structure_member_constraint(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void pointer_structure_member_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("pointer_structure_member_");
	pointer_structure_member_constraint(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void postfix_increment_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("postfix_increment_");
	left_is_qualified_or_unqualified_real_or_pointer(e__, self, tk);
	CHECK(error1);
	left_is_modifiable_lvalue(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void postfix_decrement_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("postfix_decrement_");
	left_is_qualified_or_unqualified_real_or_pointer(e__, self, tk);
	CHECK(error1);
	left_is_modifiable_lvalue(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void compound_literals_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("compound_literals_");
	compound_literals_constraint(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void prefix_increment_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("prefix_increment_");
	right_is_qualified_or_unqualified_real_or_pointer(e__, self, tk);
	CHECK(error1);
	right_is_modifiable_lvalue(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void prefix_decrement_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("prefix_decrement_");
	right_is_qualified_or_unqualified_real_or_pointer(e__, self, tk);
	CHECK(error1);
	right_is_modifiable_lvalue(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void address_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("address_");
	address_constraint(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void indirection_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("indirection_");
	right_is_pointer(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void unary_plus_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("unary_plus_");
	right_is_arithmetic(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void unary_minus_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("unary_minus_");
	right_is_arithmetic(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void bitwise_complement_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("bitwise_complement_");
	right_is_integer(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void logical_negation_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("logical_negation_");
	right_is_scalar(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void sizeof_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("sizeof_");
	sizeof_constraint(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void cast_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("cast_");
	type_is_void_or_qualified_or_unqualified_scalar(e__, self, tk);
	CHECK(error1);
	right_is_scalar(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void multiply_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("multiply_");
	right_is_arithmetic(e__, self, tk);
	CHECK(error1);
	left_is_arithmetic(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void divide_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("divide_");
	right_is_arithmetic(e__, self, tk);
	CHECK(error1);
	left_is_arithmetic(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void modulo_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("modulo_");
	right_is_integer(e__, self, tk);
	CHECK(error1);
	left_is_integer(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void addition_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("addition_");
	addition_constraint(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void substraction_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("substraction_");
	substraction_constraint(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void bitwise_shift_left_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("bitwise_shift_left_");
	both_are_integer(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void bitwise_shift_right_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("bitwise_shift_right_");
	both_are_integer(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void less_than_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("less_than_");
	less_greater_constraint(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void greater_than_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("greater_than_");
	less_greater_constraint(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void less_than_or_equal_to_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("less_than_or_equal_to_");
	less_greater_constraint(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void greater_than_or_equal_to_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("greater_than_or_equal_to_");
	less_greater_constraint(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void equal_to_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("equal_to_");
	equal_constraint(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void not_equal_to_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("not_equal_to_");
	equal_constraint(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void bitwise_and_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("bitwise_and_");
	both_are_integer(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void bitwise_exclusive_or_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("bitwise_exclusive_or_");
	both_are_integer(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void bitwise_inclusive_or_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("bitwise_inclusive_or_");
	both_are_integer(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void logical_and_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("logical_and_");
	both_are_scalar(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void logical_or_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("logical_or_");
	both_are_scalar(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void conditonal_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("conditonal_");
	left_is_scalar(e__, self, tk);
	CHECK(error1);
	right_is_conditional2(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void simple_assignment_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("simple_assignment_");
	left_is_modifiable_lvalue(e__, self, tk);
	CHECK(error1);
	simple_assignment_constraint(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void multiply_assignment_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("multiply_assignment_");
	left_is_modifiable_lvalue(e__, self, tk);
	CHECK(error1);
	right_is_arithmetic(e__, self, tk);
	CHECK(error1);
	left_is_arithmetic(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void divide_assignment_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("divide_assignment_");
	left_is_modifiable_lvalue(e__, self, tk);
	CHECK(error1);
	right_is_arithmetic(e__, self, tk);
	CHECK(error1);
	left_is_arithmetic(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void modulo_assignment_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("modulo_assignment_");
	left_is_modifiable_lvalue(e__, self, tk);
	CHECK(error1);
	right_is_arithmetic(e__, self, tk);
	CHECK(error1);
	left_is_arithmetic(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void addition_assignment_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("addition_assignment_");
	left_is_modifiable_lvalue(e__, self, tk);
	CHECK(error1);
	addition_assignment_constraint(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void substraction_assignment_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("substraction_assignment_");
	left_is_modifiable_lvalue(e__, self, tk);
	CHECK(error1);
	addition_assignment_constraint(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void shift_left_assignment_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("shift_left_assignment_");
	left_is_modifiable_lvalue(e__, self, tk);
	CHECK(error1);
	both_are_integer(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void shift_right_assignment_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("shift_right_assignment_");
	left_is_modifiable_lvalue(e__, self, tk);
	CHECK(error1);
	both_are_integer(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void and_assignment_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("and_assignment_");
	left_is_modifiable_lvalue(e__, self, tk);
	CHECK(error1);
	both_are_integer(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void exclusive_or_assignment_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("exclusive_or_assignment_");
	left_is_modifiable_lvalue(e__, self, tk);
	CHECK(error1);
	both_are_integer(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void inclusive_or_assignment_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("inclusive_or_assignment_");
	left_is_modifiable_lvalue(e__, self, tk);
	CHECK(error1);
	both_are_integer(e__, self, tk);
	CHECK(error1);
	return;
error1:
	return;
};
static void comma_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("comma_");
	return;
error1:
	return;
};

/* ************************************************************************* */

static void process_other(
	Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("process_other");

	syntax_error(e__, self, tk->token);
	CHECK(error1);
	return;
error1:
	return;
}


static void walk_tree(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)
{
	F("walk_tree");

	if (tk->left != 0) {
		walk_tree(e__, self, tk->left);
		CHECK(error1);
	}
	if (tk->right != 0) {
		walk_tree(e__, self, tk->right);
		CHECK(error1);
	}
	if (tk->type->to_basic_block != 0) {
		tk->type->to_basic_block(e__, self, tk);
		CHECK(error1);
	} else {
		process_other(e__, self, tk);
		CHECK(error1);
	}
	return;
error1:
	return;
}

static void finish(Ex **e__, cc__Expression *self)
{
	F("finish");
	cc__ExpressionTree *tk;

	
	(void)__func__;
	(void)e__;
	(void)self->env;

	tk = self->root->next;
	self->current = self->root;
	while (tk != 0) {
		place_in_tree(e__, self, tk);
		CHECK(error1);
		tk = tk->next;
	}
	if (self->root->left) {
		walk_tree(e__, self, self->root->left);
	}
	CHECK(error1);
	return;
error1:
	return;
}


static void syntax_error(Ex **e__, cc__Expression *self, cc__PreToken *token)
{
	F("syntax_error");

	THROW(-100201, "Syntax error.");
	CC__WARN("", token->file, token->line, token->char_pos);
	CHECK(error);
	return;
error:
	return;
}

INIT_EXPORT cc__Expression__class cc__Expression__  = {
	// Object
	(utf8*)"cc__Expression",
	sizeof(cc__Expression),
	0,
	__init__,
	__ctor__,
	__dtor__,
	hash_code,
	to_string,

	// Expression
	push,
	finish,

	auto_,
	break_,
	case_,
	char_,
	const_,
	continue_,
	default_,
	do_,
	double_,
	else_,
	enum_,
	extern_,
	float_,
	for_,
	goto_,
	if_,
	inline_,
	int_,
	long_,
	register_,
	restrict_,
	return_,
	short_,
	signed_,
	static_,
	struct_,
	switch_,
	typedef_,
	union_,
	unsigned_,
	void_,
	volatile_,
	while_,
	_Bool_,
	_Complex_,
	_Imaginary_,
	/**/pointer_,
	/**/object_type_,
	/**/function_type_,
	/**/label_,
	/**/typedef_identifier_,
	/**/string_constant_,
	/**/character_constant_,
	/**/number_constant_,
	paranthes_,
	plusplus_,
	minusminus_,
	and_,
	star_,
	plus_,
	minus_,
	colon_,
	semicolon_,
	bracket_,
	brace_,
	closing_brace_,
	closing_paranthesis_,
	closing_bracket_,
	array_subscripting_,
	function_calls_,
	structure_member_,
	pointer_structure_member_,
	postfix_increment_,
	postfix_decrement_,
	compound_literals_,
	prefix_increment_,
	prefix_decrement_,
	address_,
	indirection_,
	unary_plus_,
	unary_minus_,
	bitwise_complement_,
	logical_negation_,
	sizeof_,
	cast_,
	multiply_,
	divide_,
	modulo_,
	addition_,
	substraction_,
	bitwise_shift_left_,
	bitwise_shift_right_,
	less_than_,
	greater_than_,
	less_than_or_equal_to_,
	greater_than_or_equal_to_,
	equal_to_,
	not_equal_to_,
	bitwise_and_,
	bitwise_exclusive_or_,
	bitwise_inclusive_or_,
	logical_and_,
	logical_or_,
	conditonal_,
	simple_assignment_,
	multiply_assignment_,
	divide_assignment_,
	modulo_assignment_,
	addition_assignment_,
	substraction_assignment_,
	shift_left_assignment_,
	shift_right_assignment_,
	and_assignment_,
	exclusive_or_assignment_,
	inclusive_or_assignment_,
	comma_
};

