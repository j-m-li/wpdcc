#include <stdio.h> 
#include <string.h> 

#define FN(a) a, #a

#define P(a, b, c, d) ((a & 0xff)  | ((b << 8) & 0xff00) \
	| ((c << 16) & 0xff0000) | ((d << 24) & 0xff000000))
#define CC_IDENTIFIER	0x90000000

#define CC_ASSOCIATIVITY_LEFT 1
#define CC_ASSOCIATIVITY_RIGHT 0
#define CC_ASSOCIATIVITY_NONE 2

typedef struct LangDef_struct {
	char *name;
	unsigned int pre_type;
	unsigned int pre_type2;
	int precedence;
	int associativity;
	char constraints[128];
	unsigned int id;
	int done;
} LangDef;

typedef struct TypeDef_struct {
	char *name;
	unsigned int id;
} TypeDef;


#define CC__TOKEN_STORAGE_CLASS	0x80000000
#define CC__TOKEN_TYPE_SPECIFIER	0x40000000
#define CC__TOKEN_QUALIFIER	0x20000000
#define CC__TOKEN_OPERATOR	0x10000000
#define CC__TOKEN_JUMP		0x08000000
#define CC__TOKEN_TYPEDEFINE	0x04000000
#define CC__TOKEN_STATEMENT	0x02000000
#define CC__TOKEN_EXPRESSION	0x01000000
#define CC__TOKEN_IDENTIFIER	0x00800000
#define CC__TOKEN_CONSTANT	0x00400000

LangDef language[] = {
#if 0
	{"/**/unsigned_char", CC_IDENTIFIER, 0, 
		500, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_TYPE_SPECIFIER},
	{"/**/signed_char", CC_IDENTIFIER, 0, 
		500, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_TYPE_SPECIFIER},
	{"/**/unsigned_short", CC_IDENTIFIER, 0, 
		500, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_TYPE_SPECIFIER},
	{"/**/signed_short", CC_IDENTIFIER, 0, 
		500, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_TYPE_SPECIFIER},
	{"/**/unsigned_int", CC_IDENTIFIER, 0, 
		500, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_TYPE_SPECIFIER},
	{"/**/signed_int", CC_IDENTIFIER, 0, 
		500, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_TYPE_SPECIFIER},
	{"/**/unsigned_long", CC_IDENTIFIER, 0, 
		500, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_TYPE_SPECIFIER},
	{"/**/signed_long", CC_IDENTIFIER, 0, 
		500, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_TYPE_SPECIFIER},
	{"/**/unsigned_long_long", CC_IDENTIFIER, 0, 
		500, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_TYPE_SPECIFIER},
	{"/**/signed_long_long", CC_IDENTIFIER, 0, 
		500, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_TYPE_SPECIFIER},
	{"/**/", CC_IDENTIFIER, 0, 
		500, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_TYPE_SPECIFIER},
	{"/**/", CC_IDENTIFIER, 0, 
		500, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_TYPE_SPECIFIER},
	{"/**/", CC_IDENTIFIER, 0, 
		500, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_TYPE_SPECIFIER},
	{"/**/", CC_IDENTIFIER, 0, 
		500, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_TYPE_SPECIFIER},
	{"/**/", CC_IDENTIFIER, 0, 
		500, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_TYPE_SPECIFIER},
	{"/**/", CC_IDENTIFIER, 0, 
		500, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_TYPE_SPECIFIER},
	{"/**/", CC_IDENTIFIER, 0, 
		500, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_TYPE_SPECIFIER},
#endif

	{"auto", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_STORAGE_CLASS},
	{"break", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_JUMP},
	{"case", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_STATEMENT},
	{"char", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_TYPE_SPECIFIER},
	{"const", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_QUALIFIER},
	{"continue", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_JUMP},
	{"default", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_STATEMENT},
	{"do", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_STATEMENT},
	{"double", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_TYPE_SPECIFIER},
	{"else", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_STATEMENT},
	{"enum", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_TYPEDEFINE},
	{"extern", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_STORAGE_CLASS},
	{"float", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_TYPE_SPECIFIER},
	{"for", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_STATEMENT},
	{"goto", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_JUMP},
	{"if", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_STATEMENT},
	{"inline", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_STORAGE_CLASS},
	{"int", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_TYPE_SPECIFIER},
	{"long", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_TYPE_SPECIFIER},
	{"register", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_STORAGE_CLASS},
	{"restrict", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_QUALIFIER},
	{"return", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_JUMP},
	{"short", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_TYPE_SPECIFIER},
	{"signed", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_TYPE_SPECIFIER},
//	{"sizeof", CC__TOKEN_OPERATOR + 1},
	{"static", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_STORAGE_CLASS},
	{"struct", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_TYPEDEFINE},
	{"switch", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_STATEMENT},
	{"typedef", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_TYPEDEFINE},
	{"union", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_TYPEDEFINE},
	{"unsigned", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_TYPE_SPECIFIER},
	{"void", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_TYPE_SPECIFIER},
	{"volatile", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_QUALIFIER},
	{"while", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_STATEMENT},
	{"_Bool", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_TYPE_SPECIFIER},
	{"_Complex", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_TYPE_SPECIFIER},
	{"_Imaginary", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_TYPE_SPECIFIER},

	{"/**/pointer", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_TYPE_SPECIFIER},

	{"/**/object_type", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_IDENTIFIER},
	{"/**/function_type", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_IDENTIFIER},
	{"/**/label", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_IDENTIFIER},
	{"/**/typedef_identifier", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_TYPE_SPECIFIER},
	{"/**/string_constant", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_CONSTANT},
	{"/**/character_constant", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_CONSTANT},
	{"/**/number_constant", CC_IDENTIFIER, 0, 
		400, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_CONSTANT},

	{"paranthes", P('(', 0, 0, 0), 0, 350, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_OPERATOR}, 
	{"plusplus", P('+', '+', 0, 0), 0, 350, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_OPERATOR}, 
	{"minusminus", P('-', '-', 0, 0), 0, 350, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_OPERATOR}, 
	{"and", P('&', 0, 0, 0), 0, 350, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_OPERATOR}, 
	{"star", P('*', 0, 0, 0), 0, 350, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_OPERATOR}, 
	{"plus", P('+', 0, 0, 0), 0, 350, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_OPERATOR}, 
	{"minus", P('-', 0, 0, 0), 0, 350, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_OPERATOR}, 
	{"colon", P(':', 0, 0, 0), 0, 350, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_OPERATOR},
	{"semicolon", P(';', 0, 0, 0), 0, 350, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_OPERATOR},
	{"bracket", P('[', 0, 0, 0), 
		P('<', ':', 0, 0), 350, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_OPERATOR}, 
	{"brace", P('{', 0, 0, 0), 
		P('<', '%', 0, 0), 300, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_OPERATOR},
	{"closing_brace", P('}', 0, 0, 0), 
		P('%', '>', 0, 0), 300, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_OPERATOR},
	{"closing_paranthesis", P(')', 0, 0, 0), 0, 
		300, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_OPERATOR},
	{"closing_bracket", P(']', 0, 0, 0), 
		P(':', '>', 0, 0), 300, CC_ASSOCIATIVITY_NONE, "", CC__TOKEN_OPERATOR},


	{"array_subscripting", P('[', '1', 0, 0), P('<', ':', '1', 0), 200, CC_ASSOCIATIVITY_LEFT, 
		"left_is_pointer_to_object,"
		"right_is_integer", CC__TOKEN_OPERATOR}, 
	{"function_calls", P('(', '1', 0, 0), 0, 200, CC_ASSOCIATIVITY_LEFT,
		"left_is_pointer_to_function,"
		"right_arguments_agree_with_parameters", CC__TOKEN_OPERATOR}, 
	{"structure_member", P('.', 0, 0, 0), 0, 200, CC_ASSOCIATIVITY_LEFT,
		"structure_member_constraint", CC__TOKEN_OPERATOR}, 
	{"pointer_structure_member", P('-', '>', 0, 0), 0, 200, CC_ASSOCIATIVITY_LEFT,
		"pointer_structure_member_constraint", CC__TOKEN_OPERATOR}, 
	{"postfix_increment", P('+', '+', '1', 0), 0, 200, CC_ASSOCIATIVITY_LEFT,
		"left_is_qualified_or_unqualified_real_or_pointer,"
		"left_is_modifiable_lvalue", CC__TOKEN_OPERATOR}, 
	{"postfix_decrement", P('-', '-', '1', 0), 0, 200, CC_ASSOCIATIVITY_LEFT,
		"left_is_qualified_or_unqualified_real_or_pointer,"
		"left_is_modifiable_lvalue", CC__TOKEN_OPERATOR}, 
	{"compound_literals", P('(', '2', 0, 0), 0, 200, CC_ASSOCIATIVITY_NONE,
		"compound_literals_constraint", CC__TOKEN_OPERATOR}, 


	{"prefix_increment", P('+', '+', '2', 0), 0, 190, CC_ASSOCIATIVITY_RIGHT,
		"right_is_qualified_or_unqualified_real_or_pointer,"
		"right_is_modifiable_lvalue", CC__TOKEN_OPERATOR}, 
	{"prefix_decrement", P('-', '-', '2', 0), 0, 190, CC_ASSOCIATIVITY_RIGHT,
		"right_is_qualified_or_unqualified_real_or_pointer,"
		"right_is_modifiable_lvalue", CC__TOKEN_OPERATOR}, 
	{"address", P('&', '1', 0, 0), 0, 190, CC_ASSOCIATIVITY_RIGHT,
		"address_constraint", CC__TOKEN_OPERATOR}, 
	{"indirection", P('*', '1', 0, 0), 0, 190, CC_ASSOCIATIVITY_RIGHT,
		"right_is_pointer", CC__TOKEN_OPERATOR}, 
	{"unary_plus", P('+', '1', 0, 0), 0, 190, CC_ASSOCIATIVITY_RIGHT,
		"right_is_arithmetic", CC__TOKEN_OPERATOR}, 
	{"unary_minus", P('-', '1', 0, 0), 0, 190, CC_ASSOCIATIVITY_RIGHT,
		"right_is_arithmetic", CC__TOKEN_OPERATOR}, 
	{"bitwise_complement", P('~', 0, 0, 0), 0, 190, CC_ASSOCIATIVITY_RIGHT,
		"right_is_integer", CC__TOKEN_OPERATOR}, 
	{"logical_negation", P('!', 0, 0, 0), 0, 190, CC_ASSOCIATIVITY_RIGHT,
		"right_is_scalar", CC__TOKEN_OPERATOR}, 
	{"sizeof", CC_IDENTIFIER, 0, 190, CC_ASSOCIATIVITY_RIGHT,
		"sizeof_constraint", CC__TOKEN_OPERATOR},

	{"cast", P('(', '3', 0, 0), 0, 180, CC_ASSOCIATIVITY_NONE,
		"type_is_void_or_qualified_or_unqualified_scalar,"
		"right_is_scalar", CC__TOKEN_OPERATOR},

	{"multiply", P('*', '2', 0, 0), 0, 170, CC_ASSOCIATIVITY_LEFT,
		"right_is_arithmetic,"
		"left_is_arithmetic", CC__TOKEN_OPERATOR},
	{"divide", P('/', 0, 0, 0), 0, 170, CC_ASSOCIATIVITY_LEFT,
		"right_is_arithmetic,"
		"left_is_arithmetic", CC__TOKEN_OPERATOR},
	{"modulo", P('%', 0, 0, 0), 0, 170, CC_ASSOCIATIVITY_LEFT,
		"right_is_integer,"
		"left_is_integer", CC__TOKEN_OPERATOR},

	{"addition", P('+', '2', 0, 0), 0, 160, CC_ASSOCIATIVITY_LEFT,
		"addition_constraint", CC__TOKEN_OPERATOR},
	{"substraction", P('-', '2', 0, 0), 0, 160, CC_ASSOCIATIVITY_LEFT,
		"substraction_constraint", CC__TOKEN_OPERATOR},

	{"bitwise_shift_left", P('<', '<', 0, 0), 0, 150, CC_ASSOCIATIVITY_LEFT,
		"both_are_integer", CC__TOKEN_OPERATOR},
	{"bitwise_shift_right", P('>', '>', 0, 0), 0, 150, CC_ASSOCIATIVITY_LEFT,
		"both_are_integer", CC__TOKEN_OPERATOR},

	{"less_than", P('<', 0, 0, 0), 0, 140, CC_ASSOCIATIVITY_LEFT,
		"less_greater_constraint", CC__TOKEN_OPERATOR},
	{"greater_than", P('>', 0, 0, 0), 0, 140, CC_ASSOCIATIVITY_LEFT,
		"less_greater_constraint", CC__TOKEN_OPERATOR},
	{"less_than_or_equal_to", P('<', '=', 0, 0), 0, 140, CC_ASSOCIATIVITY_LEFT,
		"less_greater_constraint", CC__TOKEN_OPERATOR},
	{"greater_than_or_equal_to", P('>', '=', 0, 0), 0, 140, CC_ASSOCIATIVITY_LEFT,
		"less_greater_constraint", CC__TOKEN_OPERATOR},

	{"equal_to", P('=', '=', 0, 0), 0, 130, CC_ASSOCIATIVITY_LEFT,
		"equal_constraint", CC__TOKEN_OPERATOR},
	{"not_equal_to", P('!', '=', 0, 0), 0, 130, CC_ASSOCIATIVITY_LEFT,
		"equal_constraint", CC__TOKEN_OPERATOR},
	
	{"bitwise_and", P('&', '2', 0, 0), 0, 120, CC_ASSOCIATIVITY_LEFT,
		"both_are_integer", CC__TOKEN_OPERATOR},

	{"bitwise_exclusive_or", P('^', 0, 0, 0), 0, 110, CC_ASSOCIATIVITY_LEFT,
		"both_are_integer", CC__TOKEN_OPERATOR},

	{"bitwise_inclusive_or", P('|', 0, 0, 0), 0, 100, CC_ASSOCIATIVITY_LEFT,
		"both_are_integer", CC__TOKEN_OPERATOR},

	{"logical_and", P('&', '&', 0, 0), 0, 90, CC_ASSOCIATIVITY_LEFT,
		"both_are_scalar", CC__TOKEN_OPERATOR},

	{"logical_or", P('|', '|', 0, 0), 0, 80, CC_ASSOCIATIVITY_LEFT,
		"both_are_scalar", CC__TOKEN_OPERATOR},

	{"conditonal", P('?', 0, 0, 0), 0, 70, CC_ASSOCIATIVITY_LEFT,
		"left_is_scalar,right_is_conditional2", CC__TOKEN_OPERATOR},

	{"simple_assignment", P('=', 0, 0, 0), 0, 60, CC_ASSOCIATIVITY_RIGHT,
		"left_is_modifiable_lvalue,"
		"simple_assignment_constraint", CC__TOKEN_OPERATOR},
	{"multiply_assignment", P('*', '=', 0, 0), 0, 60, CC_ASSOCIATIVITY_RIGHT,
		"left_is_modifiable_lvalue,"
		"right_is_arithmetic,"
		"left_is_arithmetic", CC__TOKEN_OPERATOR},
	{"divide_assignment", P('/', '=', 0, 0), 0, 60, CC_ASSOCIATIVITY_RIGHT,
		"left_is_modifiable_lvalue,"
		"right_is_arithmetic,"
		"left_is_arithmetic", CC__TOKEN_OPERATOR},
	{"modulo_assignment", P('%', '=', 0, 0), 0, 60, CC_ASSOCIATIVITY_RIGHT,
		"left_is_modifiable_lvalue,"
		"right_is_arithmetic,"
		"left_is_arithmetic", CC__TOKEN_OPERATOR},
	{"addition_assignment", P('+', '=', 0, 0), 0, 60, CC_ASSOCIATIVITY_RIGHT,
		"left_is_modifiable_lvalue,"
		"addition_assignment_constraint", CC__TOKEN_OPERATOR},
	{"substraction_assignment", P('-', '=', 0, 0), 0, 60, CC_ASSOCIATIVITY_RIGHT,
		"left_is_modifiable_lvalue,"
		"addition_assignment_constraint", CC__TOKEN_OPERATOR},
	{"shift_left_assignment", P('<', '<', '=', 0), 0, 60, CC_ASSOCIATIVITY_RIGHT,
		"left_is_modifiable_lvalue,"
		"both_are_integer", CC__TOKEN_OPERATOR},
	{"shift_right_assignment", P('>', '>', '=', 0), 0, 60, CC_ASSOCIATIVITY_RIGHT,
		"left_is_modifiable_lvalue,"
		"both_are_integer", CC__TOKEN_OPERATOR},
	{"and_assignment", P('&', '=', 0, 0), 0, 60, CC_ASSOCIATIVITY_RIGHT,
		"left_is_modifiable_lvalue,"
		"both_are_integer", CC__TOKEN_OPERATOR},
	{"exclusive_or_assignment", P('^', '=', 0, 0), 0, 60, CC_ASSOCIATIVITY_RIGHT,
		"left_is_modifiable_lvalue,"
		"both_are_integer", CC__TOKEN_OPERATOR},
	{"inclusive_or_assignment", P('|', '=', 0, 0), 0, 60, CC_ASSOCIATIVITY_RIGHT,
		"left_is_modifiable_lvalue,"
		"both_are_integer", CC__TOKEN_OPERATOR},

	{"comma", P(',', 0, 0, 0), 0, 50, CC_ASSOCIATIVITY_LEFT, "", CC__TOKEN_OPERATOR}
};


char *toup(char *in)
{
	static char buf[128];
	int i = 0;
	while (in[i]) {
		if (in[i] >= 'a' && in[i] <= 'z') {
			buf[i] = in[i] - 'a' + 'A';
		} else {
			buf[i] = in[i];
		}
		i++;
	}
	buf[i] = 0;
	return buf;
}

char* get_name(char *na)
{
	if (na[0] == '/' && na[1] == '*') {
		na += 4;
	}
	return toup(na);
}

void gen_define(FILE *fo, LangDef *d, unsigned int n)
{
	char *na = d->name;
	fprintf(fo, "#define CC__TOKEN_%s\t0x%08x\r\n", get_name(na), n);	
}


void gen_type(FILE *fo)
{
	int i = 0;
	unsigned int n;
	LangDef *d;
	unsigned int ids[9] = {0};

	while (i < sizeof(language) / sizeof(*language)) {
		d = language + i;
		switch(d->id) {
		case CC__TOKEN_IDENTIFIER:
			n = ++ids[0];
			break;	
		case CC__TOKEN_STORAGE_CLASS:
			n = ++ids[1];
			break;	
		case CC__TOKEN_TYPE_SPECIFIER:
			n = ++ids[2];
			break;	
		case CC__TOKEN_QUALIFIER:
			n = ++ids[3];
			break;	
		case CC__TOKEN_OPERATOR:
			n = ++ids[4];
			break;	
		case CC__TOKEN_JUMP:
			n = ++ids[5];
			break;	
		case CC__TOKEN_TYPEDEFINE:
			n = ++ids[6];
			break;	
		case CC__TOKEN_STATEMENT:
			n = ++ids[7];
			break;	
		case CC__TOKEN_EXPRESSION:
			n = ++ids[8];
			break;
		}
		gen_define(fo, d, i);
		i++;	
	}
	fprintf(fo, "#define CC__TYPE_NB_TYPES %d\r\n\r\n", i);	

	fprintf(fo, "#define CC__TOKEN_IDENTIFIER 0x%08x\r\n", CC__TOKEN_IDENTIFIER);	
	fprintf(fo, "#define CC__TOKEN_STORAGE_CLASS 0x%08x\r\n", CC__TOKEN_STORAGE_CLASS);	
	fprintf(fo, "#define CC__TOKEN_TYPE_SPECIFIER 0x%08x\r\n", CC__TOKEN_TYPE_SPECIFIER);	
	fprintf(fo, "#define CC__TOKEN_QUALIFIER 0x%08x\r\n", CC__TOKEN_QUALIFIER);	
	fprintf(fo, "#define CC__TOKEN_OPERATOR 0x%08x\r\n", CC__TOKEN_OPERATOR);	
	fprintf(fo, "#define CC__TOKEN_JUMP 0x%08x\r\n", CC__TOKEN_JUMP);	
	fprintf(fo, "#define CC__TOKEN_TYPEDEFINE 0x%08x\r\n", CC__TOKEN_TYPEDEFINE);	
	fprintf(fo, "#define CC__TOKEN_STATEMENT 0x%08x\r\n", CC__TOKEN_STATEMENT);	
	fprintf(fo, "#define CC__TOKEN_EXPRESSION 0x%08x\r\n", CC__TOKEN_EXPRESSION);
	fprintf(fo, "#define CC__TOKEN_CONSTANT 0x%08x\r\n", CC__TOKEN_CONSTANT);	
	fprintf(fo, "#define CC__TOKEN_ASSOCIATIVITY_LEFT 0x%08x\r\n", CC_ASSOCIATIVITY_LEFT);	
	fprintf(fo, "#define CC__TOKEN_ASSOCIATIVITY_RIGHT 0x%08x\r\n", CC_ASSOCIATIVITY_RIGHT);	
	fprintf(fo, "#define CC__TOKEN_ASSOCIATIVITY_NONE 0x%08x\r\n", CC_ASSOCIATIVITY_NONE);	

}

void gen_init_type(FILE *fo, LangDef *d, unsigned int n)
{
	char *na = d->name;


	fprintf(fo, "\t{\r\n");	

	fprintf(fo, "\t\tto = new_(cc__PreToken);\r\n");
	fprintf(fo, "\t\tSTR(s, \"%s\");\r\n", na);
	fprintf(fo, "\t\twhile (s.cstr[i]) {i++;}\r\n");
	fprintf(fo, "\t\tto->identifier = Gc__malloc(e__, i + 1); i = 0;\r\n");
	fprintf(fo, "\t\twhile (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}\r\n");
	fprintf(fo, "\t\tto->identifier[i] = 0;\r\n");
	fprintf(fo, "\t\tt = self->scope->_->new_object(e__, self->scope);\r\n");
	fprintf(fo, "\t\tself->scope->_->name_object(e__, self->scope, to);\r\n");
	fprintf(fo, "\t\tt->id = CC__TOKEN_%s;\r\n", get_name(na));
	fprintf(fo, "\t\tt->associativity = %d;\r\n", d->associativity);
	fprintf(fo, "\t\tt->precedence = %d;\r\n", d->precedence);
	fprintf(fo, "\t\tt->flags = 0x%0x;\r\n", d->id);
	fprintf(fo, "\t\tt->to_basic_block = cc__Expression__.%s_;\r\n", d->name);
	fprintf(fo, "\t\tself->scope->_->end_object(e__, self->scope, &o);\r\n");
	
	

	fprintf(fo, "\t}\r\n");
	fprintf(fo, "\tself->types[%d] = t;\r\n", n);
}

LangDef *get_single_name(int offset, char* letter)
{
	int i = 0;
	LangDef *ret = 0;
	
	while (i < sizeof(language) / sizeof(*language)) {
		LangDef *d = language + i;
		char *p = d->name;
		if (d->done == 0 && p[0] != '/' && 
			(d->pre_type == CC_IDENTIFIER) && 
			!strncmp(p, letter, offset)) 
		{
			if (ret) return 0;
			ret = d;
		}
		i++;
	}
	return ret;
}

void gen_op_switch(FILE *fo)
{
	int i = 0;
	LangDef *ret = 0;

	fprintf(fo, "\tswitch(type) {\r\n");
	
	while (i < sizeof(language) / sizeof(*language)) {
		LangDef *d = language + i;
		if ((d->pre_type & CC_IDENTIFIER) == 0) 
		{
			fprintf(fo, "\tcase 0x%0x:\r\n", d->pre_type);
			if (d->pre_type2) {
				fprintf(fo, "\tcase 0x%0x:\r\n", d->pre_type2);
			}
			fprintf(fo, "\t\treturn self->types[CC__TOKEN_%s];\r\n", get_name(d->name));
		}
		i++;
	}

	fprintf(fo, "\t}\r\n");
}

void gen_switch(FILE *fo, int offset, char* letter)
{
	int i = 0;
	int j;
	LangDef *d;

	fprintf(fo, "\t");
	for (j = 0; j < offset; j++) {
		fprintf(fo, " ");
	}
	
	d = get_single_name(offset, letter);
	if (d) {
		fprintf(fo, " p = (utf8*)\"%s\"; name = name + %d;\r\n", d->name + offset, offset);
		fprintf(fo, "\t");
		for (j = 0; j < offset; j++) {
			fprintf(fo, " ");
		}
		fprintf(fo, " while (*p && *name && *p == *name) {p++; name++;}\r\n");
		fprintf(fo, "\t");
		for (j = 0; j < offset; j++) {
			fprintf(fo, " ");
		}

		fprintf(fo, " if (*name == 0 && *p == 0) return self->types[CC__TOKEN_%s];\r\n",
					 get_name(d->name));

		return;
	}	
	fprintf(fo, " switch(name[%d]) {\r\n", offset);
	
	while (i < sizeof(language) / sizeof(*language)) {
		char *p;
		d = language + i;
		p = d->name;
		if (d->done == 0 && p[0] != '/' && 
			(d->pre_type == CC_IDENTIFIER) && 
			!strncmp(p, letter, offset)) 
		{
			if (p[offset] == 0) {
				fprintf(fo, "\t");
				for (j = 0; j < offset; j++) {
					fprintf(fo, " ");
				}
				fprintf(fo, " case 0:\r\n");
				fprintf(fo, "\t");
				for (j = 0; j < offset; j++) {
					fprintf(fo, " ");
				}
				fprintf(fo, "  return self->types[CC__TOKEN_%s];\r\n",
					 get_name(p));
			} else {
				fprintf(fo, "\t");
				for (j = 0; j < offset; j++) {
					fprintf(fo, " ");
				}
				fprintf(fo, " case '%c':\r\n", p[offset]);

				letter[offset] = p[offset];
				gen_switch(fo, offset + 1, letter);
				fprintf(fo, "\t");
				for (j = 0; j < offset; j++) {
					fprintf(fo, " ");
				}
				fprintf(fo, "  break;\r\n");

			}
			d->done = 1;
		}
		i++;
	}
	fprintf(fo, "\t");
	for (j = 0; j < offset; j++) {
		fprintf(fo, " ");
	}

	fprintf(fo, " }\r\n");
}

void gen_exp(FILE *fo)
{
	int i = 0;
	int j;
	LangDef *d;
	
	while (i < sizeof(language) / sizeof(*language)) {
		char *p, *q;
		d = language + i;
		q = p = d->constraints;

			fprintf(fo, "static void %s_(Ex **e__, cc__Expression *self, cc__ExpressionTree *tk)\r\n", d->name);
			fprintf(fo, "{\r\n");
			fprintf(fo, "\tF(\"%s_\");\r\n", d->name);
		
		while (p && *p) {
			char c;
			while (*q && *q != ',') q++;
			c = *q;
			*q = 0;
			fprintf(fo, "\t%s(e__, self, tk);\r\n", p);
			fprintf(fo, "\tCHECK(error1);\r\n", p);

			if (c != 0) {
				p = q + 1;
				*q = c;
				q++;
			} else {
				break;
			}
		}
			fprintf(fo, "\treturn;\r\n", p);
			fprintf(fo, "error1:\r\n", d->name);
			fprintf(fo, "\treturn;\r\n", p);
			fprintf(fo, "};\r\n");
		i++;
	}

	i = 0;	
	while (i < sizeof(language) / sizeof(*language)) {
		char *p, *q;
		d = language + i;
		q = p = d->constraints;

		
			fprintf(fo, "\t%s_,\r\n", d->name);
		i++;
	}

	i = 0;	
	while (i < sizeof(language) / sizeof(*language)) {
		char *p, *q;
		d = language + i;
		q = p = d->constraints;

		
			fprintf(fo, "\tvoid (*%s_)(Ex **e__,\r\n\t\tcc__Expression *self, cc__ExpressionTree *tk);\r\n", d->name);

		i++;
	}


}

void gen_parser(FILE *fo)
{
	int i = 0;
	LangDef *d;
	char buf[1024];

	fprintf(fo, "static void init_root(Ex **e__, cc__Parser *self)\r\n");
	fprintf(fo, "{\r\n");
	fprintf(fo, "\tString s;\r\n");
	fprintf(fo, "\tcc__Type *t;\r\n");
	fprintf(fo, "\tcc__Type *o;\r\n");
	fprintf(fo, "\tint32 i = 0;\r\n");
	fprintf(fo, "\tcc__PreToken *to;\r\n");
	fprintf(fo, "\t(void)e__;\r\n");

	while (i < sizeof(language) / sizeof(*language)) {
		d = language + i;
		d->done = 0;
		gen_init_type(fo, d, i);
		i++;
	}
	fprintf(fo, "}\r\n\r\n");

	fprintf(fo, "static cc__Type *get_id_type(Ex **e__, cc__Parser *self, uint32 type)\r\n");
	fprintf(fo, "{\r\n");
	fprintf(fo, "\t(void)e__;\r\n");
	fprintf(fo, "\tif (type < CC__TYPE_NB_TYPES) return self->types[type];\r\n");
	fprintf(fo, "\treturn 0;\r\n");
	fprintf(fo, "}\r\n\r\n");


	fprintf(fo, "static cc__Type *get_type_of_identifier(Ex **e__, cc__Parser *self, utf8 *name)\r\n");
	fprintf(fo, "{\r\n");
	fprintf(fo, "\tutf8 *p;\r\n");
	fprintf(fo, "\t(void)e__;\r\n");

	buf[0] = 0;
	gen_switch(fo,0,buf);
	fprintf(fo, "\treturn 0;\r\n\r\n");
	fprintf(fo, "}\r\n\r\n");

	fprintf(fo, "static cc__Type *get_operator_type(Ex **e__, cc__Parser *self, uint32 type)\r\n");
	fprintf(fo, "{\r\n");
	fprintf(fo, "\t(void)e__;\r\n");
	buf[0] = 0;
	gen_op_switch(fo);
	fprintf(fo, "\treturn 0;\r\n\r\n");
	fprintf(fo, "}\r\n\r\n");


}

void gen_expression(FILE *fo)
{

	gen_exp(fo);

}

int main(int argc, char *argv[])
{
	FILE *fo;

	fo = fopen("../../src/cc/Parser_generated.c", "wb");
	if (fo == 0) return -1;
	gen_parser(fo);
	fclose(fo);

	fo = fopen("../../include/cc/Type_generated.h", "wb");
	if (fo == 0) return -1;
	gen_type(fo);
	fclose(fo);

	fo = fopen("../../src/cc/Expression_generated.c", "wb");
	if (fo == 0) return -1;
	gen_expression(fo);
	fclose(fo);

	return 0;
}