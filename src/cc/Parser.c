/* "$Id: $"
 *
 *                          25 july MMIX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <cc/Env.h>
#include <cc/Parser.h>
#include <cc/PreProcessor.h>



static void syntax_error(Ex **e__, cc__Parser *self, cc__PreToken *token);
static void push_stack(Ex **e__, cc__Parser *self, cc__PreToken *token,
	cc__Type *type, uint32 state, uint32 flags);
static void pop_stack(Ex **e__, cc__Parser *self);

#include "Parser_generated.c"

static void __init__(Ex **e__)
{
	(void)e__;
	cc__Parser__.super = &Object__;
	return;
}


static void __ctor__(Ex **e__, cc__Parser *self)
{
	F("__ctor__");

	self->env = 0;
	self->scope = new_(cc__Scope);
	CHECK(error1);	
	init_root(e__, self);
	CHECK(error2);

	self->stack = 0;
	self->stack_index = 0;
	self->alloced_stack = 0;
	push_stack(e__, self, 0, 0, CC__PARSER_ROOT, CC__PARSER_FLAGS_SCOPE);
	CHECK(error2);

	self->expr = 0;
	return;
error2:
	delete_(self->scope);
error1:
	return;
}

void delete_stack(Ex **e__, cc__ParserStack *stack)
{
	F("delete_stack");

	if (stack->token != 0) {
		delete_(stack->token);
		CHECK(error1);
	}
error1:	
	return;
}

static void __dtor__(Ex **e__, cc__Parser *self)
{
	F("__dtor__");
	cc__Scope *s;

	s = self->scope;
	while (s) {
		cc__Scope *d = s;
		s = s->parent;
		delete_(d);
	}
	CHECK(error1);
error1:

//error2:
	while (self->stack_index > 0) {
		self->stack_index--;
		delete_stack(e__, &self->stack[self->stack_index]);
	}
	Gc__free(e__, self->stack);
	CHECK(error3);
error3:	

	if (self->expr != 0) {
		delete_(self->expr);
	}
	CHECK(error4);
error4:
	return;
}

static uint32 hash_code(Ex **e__, cc__Parser *self)
{
	(void)e__;
#ifdef _MSC_VER
	return *((uint32*)&self) >> 4;
#else 
	return (uint32)self >> 4;
#endif
} 

static void to_string(Ex **e__, cc__Parser *self, String *out)
{
	F("to_string");
	String s;

	STR(s, self->_->__name__);
	out->_->add(e__, out, &s);
	CHECK(error);
error:
	return;
}


static cc__Type *get_string_type(Ex **e__, cc__Parser *self)
{
	(void)e__;
	return self->types[CC__TOKEN_STRING_CONSTANT];
}

static cc__Type *get_character_type(Ex **e__, cc__Parser *self)
{
	(void)e__;
	return self->types[CC__TOKEN_CHARACTER_CONSTANT];
}

static cc__Type *get_number_type(Ex **e__, cc__Parser *self)
{
	(void)e__;
	return self->types[CC__TOKEN_NUMBER_CONSTANT];
}

static cc__Type *get_identifier_type(Ex **e__, cc__Parser *self, utf8 *name)
{
	F("get_identifier_type");
	String s;
	cc__Type *type;

	STR(s, name);
	type = get_type_of_identifier(e__, self, name);
	if (type != 0) return type;

	type = self->scope->_->get_type_of(e__, self->scope, &s);
	CHECK(error);
	return type;
error:
	return 0;
}

static void parse_root(Ex **e__, cc__Parser *self, cc__Type *type)
{
	F("parse_root");
	
	if (type == 0) {
		syntax_error(e__, self, self->token);
		CHECK(error);
	} else if ((type->flags & CC__TOKEN_TYPE_SPECIFIER) ==
		CC__TOKEN_TYPE_SPECIFIER)
	{
		self->scope->_->add_return_type(e__, self->scope, type);
		CHECK(error);
		push_stack(e__, self, 0, 0, 
			CC__PARSER_OBJECT_OR_FUNCTION,
			0);
		CHECK(error);
	} else {
		syntax_error(e__, self, self->token);
		CHECK(error);
	}
	return;
error:
	return;
}


static void parse_object_or_function(Ex **e__, cc__Parser *self,
	cc__Type *type)
{
	F("parse_object_or_function");
	
	if (type == 0) {
		self->scope->_->name_object(e__, self->scope, self->token);
		CHECK(error);
		self->token = 0;
		push_stack(e__, self, 0, 0, 
			CC__PARSER_RESOLVE_OBJECT_OR_FUNCTION,
			0);
		CHECK(error);
	} else if ((type->flags & CC__TOKEN_TYPE_SPECIFIER) ==
		CC__TOKEN_TYPE_SPECIFIER)
	{
		self->scope->_->add_return_type(e__, self->scope, type);
		CHECK(error);
	} else {
		syntax_error(e__, self, self->token);
		CHECK(error);
	}
	return;
error:
	return;
}

static void parse_resolve_object_or_function(Ex **e__, cc__Parser *self,
	cc__Type *type)
{
	F("parse_resolve_object_or_function");
//ECHO("OOOOORR");
//ECHO(token->identifier);
//ECHO_NUM(type->id);
	if (type != 0 && type->id == CC__TOKEN_SIMPLE_ASSIGNMENT) {
		self->scope->_->set_type_id(e__, self->scope,
			CC__TOKEN_OBJECT_TYPE);
		CHECK(error);
		// FIXME: implement
	} else if (type != 0 && type->id == CC__TOKEN_PARANTHES) {
		self->scope->_->set_type_id(e__, self->scope,
			CC__TOKEN_FUNCTION_TYPE);
		CHECK(error);
		pop_stack(e__, self);
		CHECK(error);
		
		push_stack(e__, self, 0, 0, 
			CC__PARSER_FUNCTION_PARAMETERS, 
			CC__PARSER_FLAGS_SCOPE);
		CHECK(error);
	} else {
		syntax_error(e__, self, self->token);
		CHECK(error);
	}
	return;
error:
	return;
}

static void parse_function_parameters(Ex **e__, cc__Parser *self,
	cc__Type *type)
{
	F("parse_function_parameters");

	if (type == 0) {
		if (self->scope->paranthesis < 2) {
			self->scope->_->name_object(e__,
				self->scope, self->token);
			CHECK(error);
			self->token = 0;
		}
	} else if ((type->flags & CC__TOKEN_TYPE_SPECIFIER) ==
		CC__TOKEN_TYPE_SPECIFIER) 
	{
		self->scope->_->add_return_type(e__,
			self->scope, type);
		CHECK(error);
	} else if (type->id == CC__TOKEN_STAR) {
		if (self->scope->last_rett_id == CC__TOKEN_PARANTHES) {
			type = get_id_type(e__, self, CC__TOKEN_FUNCTION_TYPE);
		} else {
			type = get_id_type(e__, self, CC__TOKEN_POINTER);
		}
		self->scope->_->add_return_type(e__,
			self->scope, type);
		CHECK(error);
	} else if (type->id == CC__TOKEN_BRACKET) {
		self->scope->bracket++;
		self->scope->_->add_return_type(e__,
			self->scope, type);
		CHECK(error);
	} else if (type->id == CC__TOKEN_CLOSING_BRACKET) {
		self->scope->_->add_return_type(e__,
			self->scope, type);
		CHECK(error);
		self->scope->bracket--;
	} else if (type->id == CC__TOKEN_PARANTHES) {
		self->scope->paranthesis++;
		self->scope->_->add_return_type(e__,
			self->scope, type);
		CHECK(error);	
	} else if (type->id == CC__TOKEN_COMMA) {
		cc__Type *o;

		if (self->scope->paranthesis == 0) {
			self->scope->_->end_parameter(e__, self->scope, &o);
			CHECK(error);
			if (o) {
				delete_(o);
				CHECK(error);
				syntax_error(e__, self, self->token);
				CHECK(error);
			}
		} else {
			self->scope->_->add_return_type(e__,
				self->scope, type);
			CHECK(error);	
		}
	} else if (type->id == CC__TOKEN_CLOSING_PARANTHESIS) {
		cc__Type *o;
		
		if (self->scope->paranthesis == 0) {
			self->scope->_->end_parameter(e__, self->scope, &o);
			CHECK(error);
			if (o) {
				delete_(o);
				CHECK(error);
				syntax_error(e__, self, self->token);
				CHECK(error);
			}
			pop_stack(e__, self);
			CHECK(error);
			push_stack(e__, self, 0, 0, 
				CC__PARSER_RESOLVE_FUNCTION_BODY, 
				0);
			CHECK(error);
		} else {
			self->scope->paranthesis--;
			self->scope->_->add_return_type(e__,
				self->scope, type);
			CHECK(error);	
		}
	} else {
		syntax_error(e__, self, self->token);
		CHECK(error);
	}
	return;
error:
	return;
}

static void parse_expression(Ex **e__, cc__Parser *self,
	cc__Type *type)
{
	F("parse_expression");

	if (self->expr == 0) {
		self->expr = new_(cc__Expression);
		CHECK(error);
		self->expr->env = self->env;
	}
	if (type != 0 && type->id == CC__TOKEN_SEMICOLON) {
		self->expr->_->finish(e__, self->expr);
		CHECK(error);
		pop_stack(e__, self);
		CHECK(error);
		delete_(self->expr);
		CHECK(error);
		self->expr = 0;
	} else {
		self->expr->_->push(e__, self->expr, type, self->token);
		CHECK(error);
		self->token = 0;
	}
	return;
error:
	return;
}

static void parse_scope_body(Ex **e__, cc__Parser *self,
	cc__Type *type)
{
	F("parse_scope_body");
ECHO("E-:HERE!!\r\n");

	if (type == 0) {
		syntax_error(e__, self, self->token);
		CHECK(error);
	} else {
		switch(type->flags) {
		case CC__TOKEN_CONSTANT:
			syntax_error(e__, self, self->token);
			CHECK(error);	
			break;
		case CC__TOKEN_TYPE_SPECIFIER:
			break;
		case CC__TOKEN_QUALIFIER:
			break;
		case CC__TOKEN_STATEMENT:
			break;
		case CC__TOKEN_TYPEDEFINE:
			switch (type->id) {
			case CC__TOKEN_ENUM:
			case CC__TOKEN_STRUCT:
			case CC__TOKEN_TYPEDEF:
			case CC__TOKEN_UNION:
				break;
			}
			break;
		case CC__TOKEN_STORAGE_CLASS:
			break;
		case CC__TOKEN_OPERATOR:
			switch (type->id) {
			case CC__TOKEN_RETURN:
				break;
			case CC__TOKEN_SEMICOLON:
				break;
			case CC__TOKEN_BRACE:
				push_stack(e__, self, 0, 0, 
					CC__PARSER_SCOPE_BODY, 
					CC__PARSER_FLAGS_SCOPE);
					CHECK(error);
				break;
			case CC__TOKEN_CLOSING_BRACE:
				pop_stack(e__, self);
				CHECK(error);
				break;
			case CC__TOKEN_PARANTHES:
				break;
			case CC__TOKEN_PLUSPLUS:
				type->id = CC__TOKEN_PREFIX_INCREMENT;
				break;
			case CC__TOKEN_MINUSMINUS:
				type->id = CC__TOKEN_PREFIX_DECREMENT;
				break;
			case CC__TOKEN_AND:
				type->id = CC__TOKEN_ADDRESS;
				break;
			case CC__TOKEN_STAR:
				type->id = CC__TOKEN_INDIRECTION;
				break;
			default:
				syntax_error(e__, self, self->token);
				CHECK(error);
			}
			break;
		case CC__TOKEN_JUMP:
			switch (type->id) {
			case CC__TOKEN_RETURN:
				push_stack(e__, self, 0, 0, 
					CC__PARSER_RETURN_EXPRESSION, 
					0);
					CHECK(error);
				break;
			case CC__TOKEN_GOTO:
				break;
			default:
				syntax_error(e__, self, self->token);
				CHECK(error);
			}
			break;
		case CC__TOKEN_IDENTIFIER:
			break;
		default:
			syntax_error(e__, self, self->token);
			CHECK(error);
		}
	}
	return;
error:
	return;
}


static void parse_return_expression(Ex **e__, cc__Parser *self,
	cc__Type *type)
{
	F("parse_return_expression");

	parse_expression(e__, self, type);
	CHECK(error);
	return;
error:
	return;
}

static void parse_function_body(Ex **e__, cc__Parser *self,
	cc__Type *type)
{
	F("parse_function_body");

	parse_scope_body(e__, self, type);
	if (type != 0 && type->id == CC__TOKEN_CLOSING_BRACE) {
		self->scope->_->end_of_function(e__, self->scope);
		CHECK(error);
	}
	return;
error:
	return;
}

static void parse_resolve_function_body(Ex **e__, cc__Parser *self, 
	cc__Type *type)
{
	F("parse_resolve_function_body");
	
	if (type == 0) {
		syntax_error(e__, self, self->token);
		CHECK(error);
	} else if (type->id  == CC__TOKEN_BRACE) {
		pop_stack(e__, self);
		CHECK(error);

		push_stack(e__, self, 0, 0, 
			CC__PARSER_FUNCTION_BODY, 
			CC__PARSER_FLAGS_SCOPE);
		CHECK(error);
	} else if (type->id  == CC__TOKEN_SEMICOLON) {
		cc__Type *t = 0;

		pop_stack(e__, self);
		CHECK(error);
		self->scope->_->end_object(e__, self->scope, &t);
		if (t != 0) {
			delete_(t);
			CHECK(error);
			syntax_error(e__, self, self->token);
			CHECK(error);
		}
	} else {
		syntax_error(e__, self, self->token);
		CHECK(error);
	}
	return;
error:
	return;
}
static void parse_token(Ex **e__, cc__Parser *self, cc__Type *type)
{
	F("parse_token");
	cc__ParserStack *s;

	s = &self->stack[self->stack_index - 1];
	switch (s->state) {
	case CC__PARSER_ROOT:
		parse_root(e__, self, type);
		break;
	case CC__PARSER_OBJECT_OR_FUNCTION:
		parse_object_or_function(e__, self, type);
		break;
	case CC__PARSER_RESOLVE_OBJECT_OR_FUNCTION:
		parse_resolve_object_or_function(e__, self, type);
		break;
	case CC__PARSER_FUNCTION_PARAMETERS:
		parse_function_parameters(e__, self, type);
		break;
	case CC__PARSER_RESOLVE_FUNCTION_BODY:
		parse_resolve_function_body(e__, self, type);
		break;
	case CC__PARSER_FUNCTION_BODY:
		parse_function_body(e__, self, type);
		break;
	case CC__PARSER_SCOPE_BODY:
		parse_scope_body(e__, self, type);
		break;
	case CC__PARSER_RETURN_EXPRESSION:
		parse_return_expression(e__, self, type);
		break;
	case CC__PARSER_EXPRESSION:
		parse_expression(e__, self, type);
		break;
	default:
		syntax_error(e__, self, self->token);
		CHECK(error);
	}
	CHECK(error);
	return;
error:
	return;
}

static void parse(Ex **e__, cc__Parser *self, cc__PreTokens *tokens)
{
	F("parse");
	cc__Type *type;

	tokens->_->get_first_token(e__, tokens, &self->token);
	CHECK(error);
	while (self->token) {
		cc__PreToken *t = self->token;

		type = 0;
	        ECHO(t->identifier);
		ECHO(" ");
		
	
		if ((t->type & CC_WHITE_SPACE) == CC_WHITE_SPACE) {
			/* ignore white spaces */
			type = (void*)-1;
		} else if ((t->type & CC_IDENTIFIER) == CC_IDENTIFIER) { 
			type = get_identifier_type(e__, self, t->identifier);
			CHECK(error);
		} else if ((t->type & CC_ISNOT_PUNCTUATOR) == 0) {
			type = get_operator_type(e__, self, t->type);
			CHECK(error);
		} else if (t->type == CC_STRING) {
			type = get_string_type(e__, self);
			CHECK(error);
		} else if (t->type == CC_CHARACTER) {
			type = get_character_type(e__, self);
			CHECK(error);
		} else if ((t->type & CC_NUMBER) == CC_NUMBER) {	
			type = get_number_type(e__, self);
			CHECK(error);
		} 

		if (type != (void*)-1) {
			parse_token(e__, self, type);
			CHECK(error);
		}
		
		//ECHO_NUM(self->stack[self->stack_index - 1].state);
		//ECHO(" ");
		if (self->token) delete_(self->token);
		tokens->_->get_first_token(e__, tokens, &self->token);
	}
	CHECK(error);
	return;
error:
	{
		Ex *e = 0;
		self->scope->_->dump(&e, self->scope);
	}
	if (self->token) delete_(self->token);
	return;
}



static void syntax_error(Ex **e__, cc__Parser *self, cc__PreToken *token)
{
	F("syntax_error");

	THROW(-100200, "Syntax error.");
	CC__WARN("", token->file, token->line, token->char_pos);
	CHECK(error);
	return;
error:
	return;
}

static void pop_stack(Ex **e__, cc__Parser *self)
{
	F("pop_stack");
	cc__ParserStack *t;

	if (self->stack_index <= 0) return;
	self->stack_index--;
	t = &self->stack[self->stack_index];
	if (t->token != 0) {
//		delete_(t->token);
		CHECK(error);
	}
	if ((t->flags & CC__PARSER_FLAGS_SCOPE) != 0) {
		cc__Scope *s;
		ECHO("\njjj\n");
		s = self->scope->_->end_scope(e__, self->scope);
		CHECK(error);
		self->scope = s;
	}
	return;
error:
	return;
}

static void push_stack(Ex **e__, cc__Parser *self, cc__PreToken *token,
	cc__Type *type, uint32 state, uint32 flags)
{
	F("push_stack");
	cc__ParserStack *t;
	cc__ParserStack *ns = 0;

	if (self->stack_index >= self->alloced_stack) {
		int32 i = self->stack_index;
		
		self->alloced_stack += 256;
		ns = Gc__malloc(e__, self->alloced_stack * 
			sizeof(cc__ParserStack));
		CHECK(error1);
		while (i > 0) {
			i--;
			ns[i] = self->stack[i];
		}
		Gc__free(e__, self->stack);
		CHECK(error2);
		self->stack = ns;
	}
	t = &self->stack[self->stack_index];
	self->stack_index++;
	t->flags = flags;
	t->token = token;
	t->type = type;
	t->state = state;

	if ((flags & CC__PARSER_FLAGS_SCOPE) != 0) {
		cc__Scope *s;

		s = self->scope->_->new_scope(e__, self->scope);
		CHECK(error1);
		self->scope = s;
	}
	return;
error2:
	Gc__free(e__, ns);
error1:
	return;
}

static cc__Type *get_composite_type(Ex **e__, cc__Parser *self, 
		uint32 *types, uint32 nb_types)
{
	F("get_composite_type");
	cc__Type *t = 0;
	String s;
	cc__Type *o;
	uint32 i = 0;
	cc__PreToken *to;
	cc__Scope *root = self->scope;

	while (root->parent) {
		root = root->parent;
	}
	{
		cc__Type *old;
		to = new_(cc__PreToken);
		i = nb_types * 2;
		to->identifier = Gc__malloc(e__, i + 2);
		to->identifier[i+1] = 0;
		to->identifier[i] = '#'; 
		i = nb_types;
		while (i > 0) {
			i--;
			to->identifier[i << 1] = (types[i] & 0x0F) + 'a';
			to->identifier[(i << 1) + 1] = 
				((types[i] & 0xF0) >> 4) + 'a';
		}
		old = root->current_object;
		root->current_object = 0;
		t = root->_->new_object(e__, root);
		root->_->name_object(e__, root, to);
		t->id = 0xFFFFFFFF;
		t->associativity = CC__TOKEN_ASSOCIATIVITY_NONE;
		t->precedence = 400;
		t->flags = CC__TOKEN_TYPE_SPECIFIER;
		t->to_basic_block = 0;
		i = 0;
		while (i < nb_types) {
			root->current_object->_->add_sub_type(e__,
				root->current_object, 
				get_id_type(e__, self, types[i]));
			i++;
		}

		root->_->end_object(e__, root, &o);
		root->current_object = old;
		if (o) {
			delete_(o);
		}
	}
	CHECK(error1);
	return t;
error1:
	return 0;
}

INIT_EXPORT cc__Parser__class cc__Parser__  = {
	// Object
	(utf8*)"cc__Parser",
	sizeof(cc__Parser),
	0,
	__init__,
	__ctor__,
	__dtor__,
	hash_code,
	to_string,

	// Parser
	parse,
	get_id_type,
	get_composite_type
};

