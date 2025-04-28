/* "$Id: $"
 *
 *                          21 June MMVIII PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#ifndef CC__PREPROCESSOR_H
#define CC__PREPROCESSOR_H

#include <lang/Buffer.h>
#include <lang/Array.h>
#include <cc/PreTokens.h>
#include <cc/PreMacros.h>
#include <lang/Stack.h>


#define CC_WHITE_SPACE	0xC0000000
#define CC_COMMENT	0xC0000001
#define CC_LINE_COMMENT	0xC0000002
#define CC_LINE		0xC0000003
#define CC_FILE		0xC0000004
#define CC_PRAGMA	0xC0000005
#define CC_END_TOKEN	0xC0000006

#define CC_ISNOT_PUNCTUATOR	0x80000000
#define CC_INVALID_CHAR	0x80000000
#define CC_IDENTIFIER	0x90000000
#define CC_EXPANDED_ID	0x90000001
#define CC_PLACEMARKER	0x90000002
#define CC_STRING	0x88000000
#define CC_CHARACTER	0x84000000
#define CC_INVALID	0x02000000
#define CC_NUMBER 	0xA0000000
#define CC_DECIMAL 		0xA0000001
#define CC_OCTAL 		0xA0000002
#define CC_HEXADECIMAL 		0xA0000003
#define CC_DECIMAL_L 		0xA0000011
#define CC_OCTAL_L 		0xA0000012
#define CC_HEXADECIMAL_L 	0xA0000013
#define CC_DECIMAL_LL 		0xA0000031
#define CC_OCTAL_LL 		0xA0000032
#define CC_HEXADECIMAL_LL 	0xA0000033
#define CC_DECIMAL_U 		0xA0000041
#define CC_OCTAL_U 		0xA0000042
#define CC_HEXADECIMAL_U 	0xA0000043
#define CC_DECIMAL_UL 		0xA0000051
#define CC_OCTAL_UL 		0xA0000052
#define CC_HEXADECIMAL_UL 	0xA0000053
#define CC_DECIMAL_ULL 		0xA0000071
#define CC_OCTAL_ULL 		0xA0000072
#define CC_HEXADECIMAL_ULL 	0xA0000073
#define CC_LONG			0x00000010
#define CC_LONG_LONG		0x00000020
#define CC_UNSIGNED		0x00000040
#define CC_FLOAT		0x00000080
#define CC_DOUBLE		0x00000100
#define CC_LONG_DOUBLE		0x00000200

typedef struct cc__PreProcessor {

	// Object
	struct cc__PreProcessor__class *_;

	// String
	int32 size;
	utf8 *cstr;	

	// Buffer
	int32 alloced;

	// PreProcessor
	int8 in_comment;
	int8 has_sharp;

	cc__PreTokens *tokens;
	cc__PreMacros *macros;

	uint32 *cstr_char_line;
	uint32 *cstr_char_pos;
	int32 cstr_char_alloced; 

	cc__Env *env;
	io__File *f;
	int32 offset;
} cc__PreProcessor;

typedef struct cc__PreProcessor__class {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct Buffer__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, cc__PreProcessor *self);
	void (*__dtor__)(Ex **e__, cc__PreProcessor *self);
	uint32 (*hash_code)(Ex **e__, cc__PreProcessor *self); 
	void (*to_string)(Ex **e__, cc__PreProcessor *self, String *out);

	// String
	void (*io__i_serializable)(Ex **e__,
		cc__PreProcessor *self, io__ISerializable *out);
	void (*serialize)(Ex **e__, io__ISerializable *itf,
		io__Serializer *out);
	void (*unserialize)(Ex **e__, io__ISerializable *itf,
		io__Serializer *out);
	void (*i_comparable)(Ex **e__, cc__PreProcessor *self,
		IComparable *out);
	int8 (*compare_to)(Ex **e__, IComparable *itf, Object *in);
	void (*add)(Ex **e__, cc__PreProcessor *self, String *in);
	void (*set)(Ex **e__, cc__PreProcessor *self, String *in);
	void (*clear)(Ex **e__, cc__PreProcessor *self);
	utf8 *(*get_cstr)(Ex **e__, cc__PreProcessor *self);
	int32 (*get_size)(Ex **e__, cc__PreProcessor *self);
	void (*printf)(Ex **e__, cc__PreProcessor *self, String *format, ...);
	int32 (*index_of)(Ex **e__, cc__PreProcessor *self, String *needle);

	// Buffer
	void (*add_data)(Ex **e__, cc__PreProcessor *self, utf8 *data,
		int32 size);

	// PreProcessor
	void (*process_file)(Ex **e__, cc__PreProcessor *self, String *file);
	void (*set_env)(Ex **e__, cc__PreProcessor *self, cc__Env *env);
	uint32 (*get_type_of_cstr)(Ex **e__, cc__PreProcessor *self,
		utf8 *cstr, int32 size);
	void (*run)(Ex **e__, cc__PreProcessor *self);
	int8 (*process_loop)(Ex **e__, cc__PreProcessor *self);
	void (*process)(Ex **e__, cc__PreProcessor *self);
} cc__PreProcessor__class; 

EXPORT cc__PreProcessor__class cc__PreProcessor__;

EXPORT void cc__PreProcessor__process(Ex **e__, cc__PreProcessor *self);

#endif // CC__PREPROCESSOR_H
