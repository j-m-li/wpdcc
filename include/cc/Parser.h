/* "$Id: $"
 *
 *                          25 july MMIX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#ifndef CC__PARSER_H
#define CC__PARSER_H

#include <lang/Object.h>
#include <lang/Array.h>
#include <lang/Buffer.h>
#include <lang/Stack.h>
#include <cc/Env.h>
#include <cc/PreTokens.h>
#include <cc/Scope.h>
#include <cc/Type.h>
#include <cc/Expression.h>

/* root of the current translation unit, this is the first state pushed onto
 the stack at the begining of the parsing */
#define CC__PARSER_ROOT 0
/* declaration or definition of a variable or a function */
#define CC__PARSER_OBJECT_OR_FUNCTION 1
/* the next token should resolve the type of the object */
#define CC__PARSER_RESOLVE_OBJECT_OR_FUNCTION 2
/* we are in function decaration parameter list */
#define CC__PARSER_FUNCTION_PARAMETERS 3
/* the next token should tell if there is a function body */
#define CC__PARSER_RESOLVE_FUNCTION_BODY 4
/* we are in a function body */
#define CC__PARSER_FUNCTION_BODY 5
/* we are in a sub scope inside a function */
#define CC__PARSER_SCOPE_BODY 6
/* we are in a return statement */
#define CC__PARSER_RETURN_EXPRESSION 7
/* we are in an expression */
#define CC__PARSER_EXPRESSION 8


/* the object is referenced in a function */
#define CC__PARSER_FLAGS_USED		0x80000000
/* a new context scope is needed */
#define CC__PARSER_FLAGS_SCOPE		0x40000000
/* first token of a type specifier or things like that */
#define CC__PARSER_FLAGS_BEGIN_TOKEN	0x20000000

typedef struct cc__ParserStack {
	cc__PreToken *token;
	cc__Type *type;
	uint32 state;
	uint32 flags;
} cc__ParserStack;


typedef struct cc__Parser {
	// Object
	struct cc__Parser__class *_;

	// Parser
	struct cc__Env *env;
	cc__Scope *scope;
	cc__ParserStack *stack;
	int32 stack_index;
	int32 alloced_stack;
	cc__Type *types[CC__TYPE_NB_TYPES];
	cc__PreToken *token;
	struct cc__Expression *expr;
} cc__Parser;

typedef struct cc__Parser__class {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct Object__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, cc__Parser *self);
	void (*__dtor__)(Ex **e__, cc__Parser *self);
	uint32 (*hash_code)(Ex **e__, cc__Parser *self); 
	void (*to_string)(Ex **e__, cc__Parser *self, String *out);

	// Parser
	void (*parse)(Ex **e__, cc__Parser *self, cc__PreTokens *tokens);
	cc__Type *(*get_id_type)(Ex **e__, cc__Parser *self, uint32 type);
	cc__Type *(*get_composite_type)(Ex **e__, cc__Parser *self, 
		uint32 *types, uint32 nb_types);

} cc__Parser__class; 

EXPORT cc__Parser__class cc__Parser__;

#endif // CC__PARSER_H
