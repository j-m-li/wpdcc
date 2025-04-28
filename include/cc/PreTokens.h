/* "$Id: $"
 *
 *                          24 July MMVIII PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#ifndef CC__PRETOKENS_H
#define CC__PRETOKENS_H

#include <lang/Object.h>
#include <cc/PreToken.h>

typedef struct cc__PreTokens {
	// Object
	struct cc__PreTokens__class *_;

	// PreTokens
	cc__PreToken *first;
	cc__PreToken *last;
	cc__PreToken *next_to_expand_token;
	cc__PreToken *next_to_execute_pragma;
} cc__PreTokens;

typedef struct cc__PreTokens__class {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct Object__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, cc__PreTokens *self);
	void (*__dtor__)(Ex **e__, cc__PreTokens *self);
	uint32 (*hash_code)(Ex **e__, cc__PreTokens *self); 
	void (*to_string)(Ex **e__, cc__PreTokens *self, String *out);

	// PreTokens
	void (*push)(Ex **e__, cc__PreTokens *self, utf8 *name, 
		int32 size, uint32 type, uint32 file, uint32 line,
		uint32 char_pos);
	void (*replace)(Ex **e__, cc__PreTokens *self,
		cc__PreToken *from, cc__PreToken *to, cc__PreTokens *by);
	void (*copy)(Ex **e__, cc__PreTokens *self, cc__PreTokens **out);
	void (*delete_end)(Ex **e__, cc__PreTokens *self, cc__PreToken *from);
	void (*delete_token)(Ex **e__, cc__PreTokens *self,
		cc__PreToken *token);
	void (*get_pos)(Ex **e__, cc__PreTokens *self, cc__PreToken *token, 
		uint32 *file, uint32 *line, uint32 *char_pos);
	void (*extract)(Ex **e__, cc__PreTokens *self,
		cc__PreToken *from, cc__PreToken *to, cc__PreTokens **out);
	void (*get_first_token)(Ex **e__, cc__PreTokens *self,
		cc__PreToken **out);
} cc__PreTokens__class; 

EXPORT cc__PreTokens__class cc__PreTokens__;

EXPORT void cc__PreTokens__push(Ex **e__, cc__PreTokens *self, utf8 *name, 
		int32 size, uint32 type, uint32 file, uint32 line,
		uint32 char_pos);


#endif // CC__PRETOKENS_H

