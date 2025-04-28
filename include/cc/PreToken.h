/* "$Id: $"
 *
 *                          24 July MMVIII PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#ifndef CC__PRETOKEN_H
#define CC__PRETOKEN_H

#include <lang/Object.h>

typedef struct cc__PreToken {
	// Object
	struct cc__PreTokens__class *_;

	// PreToken
	utf8 *identifier;
	uint32 type;
	struct cc__PreToken *next;
	struct cc__PreToken *previous;
	uint32 file;
	uint32 line;
	uint32 char_pos;
	int32 replace_index;
} cc__PreToken;

typedef struct cc__PreToken__class {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct Object__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, cc__PreToken *self);
	void (*__dtor__)(Ex **e__, cc__PreToken *self);
	uint32 (*hash_code)(Ex **e__, cc__PreToken *self); 
	void (*to_string)(Ex **e__, cc__PreToken *self, String *out);

	// PreToken
} cc__PreToken__class; 

EXPORT cc__PreToken__class cc__PreToken__;

#endif // CC__PRETOKEN_H

