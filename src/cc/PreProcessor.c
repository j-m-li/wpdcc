/* "$Id: $"
 *
 *                          21 June MMVIII PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <cc/PreProcessor.h>
#include <cc/PreExpression.h>
#include <cc/Env.h>
#include <io/File.h>

#define IS_WHITE_SPACE(c) ( \
	c == ' ' || \
	c == '\t' || \
	c == '\r' || \
	c == '\n' || \
	c == '\v' \
)

#define P(a, b, c, d) ((a & 0xff)  | ((b << 8) & 0xff00) \
	| ((c << 16) & 0xff0000) | ((d << 24) & 0xff000000))



typedef struct MacroName {
	utf8 *name;
	struct MacroName *prev;
} MacroName;

static void expand_macros(Ex **e__, cc__PreProcessor *self,
	cc__PreTokens *tokens);

static IComparable__class IComparable__;
static io__ISerializable__class io__ISerializable__;

static void __init__(Ex **e__)
{
	(void)e__;

	cc__PreProcessor__.super = &Buffer__;

	INIT_INTERFACE(IComparable, cc__PreProcessor);
	INIT_INTERFACE(io__ISerializable, cc__PreProcessor);
	ASSIGN_FUNC(Buffer, set, cc__PreProcessor);
	ASSIGN_FUNC(Buffer, clear, cc__PreProcessor);
	ASSIGN_FUNC(Buffer, get_cstr, cc__PreProcessor);
	ASSIGN_FUNC(Buffer, get_size, cc__PreProcessor);
	ASSIGN_FUNC(Buffer, printf, cc__PreProcessor);
	ASSIGN_FUNC(Buffer, index_of, cc__PreProcessor);
	ASSIGN_FUNC(Buffer, add_data, cc__PreProcessor);
	return;
}

static void __ctor__(Ex **e__, cc__PreProcessor *self)
{
	F("__ctor__");

	self->in_comment = 0;
	self->has_sharp = 0;

	self->tokens = new_(cc__PreTokens);
	CHECK(error2);
	self->macros = new_(cc__PreMacros);
	CHECK(error3);
	self->cstr_char_alloced = 1024;
	self->cstr_char_line = Gc__malloc(e__,
		self->cstr_char_alloced * sizeof(uint32));
	CHECK(error5);
	self->cstr_char_pos = Gc__malloc(e__,
		self->cstr_char_alloced * sizeof(uint32));
	CHECK(error6);

	self->env = 0;
	return;
error6:
	Gc__free(e__, self->cstr_char_line);
error5:
	delete_(self->macros);
error3:
	delete_(self->tokens);
error2:
//error1:
	return;
}

static void __dtor__(Ex **e__, cc__PreProcessor *self)
{
	F("__dtor__");

	delete_(self->tokens);
	CHECK(error3);
error3:
	delete_(self->macros);
	CHECK(error5);
error5:
	Gc__free(e__, self->cstr_char_line);
	CHECK(error6);
error6:
	Gc__free(e__, self->cstr_char_pos);
	CHECK(error7);
error7:
	return;
}

static uint32 hash_code(Ex **e__, cc__PreProcessor *self)
{
	(void)e__;
#ifdef _MSC_VER
	return *((uint32*)&self) >> 4;
#else 
	return (uint32)self >> 4;
#endif
} 

static void to_string(Ex **e__, cc__PreProcessor *self, String *out)
{
	F("to_string");
	String s;

	STR(s, self->_->__name__);
	out->_->add(e__, out, &s);
	CHECK(error);
error:
	return;
}

static void resize_cstr_char(Ex **e__, cc__PreProcessor *self, uint32 ol,
	uint32 nl)
{
	F("resize_cstr_char");
	uint32 *new_line;
	uint32 *new_pos;
	uint32 *old_line;
	uint32 *old_pos;
	uint32 i;
	
	nl += 512;

	old_line = self->cstr_char_line; 
	new_line = Gc__malloc(e__, nl * sizeof(uint32));
	CHECK(error1);
	old_pos = self->cstr_char_pos;
	new_pos = Gc__malloc(e__, nl * sizeof(uint32));
	CHECK(error2);

	for (i = 0; i < ol; i++) {
		new_line[i] = old_line[i];
		new_pos[i] = old_pos[i];
	}

	Gc__free(e__, old_pos);
	CHECK(error3);
	self->cstr_char_pos = new_pos;
	Gc__free(e__, old_line);
	CHECK(error2);
	self->cstr_char_line = new_line;
	self->cstr_char_alloced = nl;
	return;
error3:
	Gc__free(e__, new_pos);
error2:
	Gc__free(e__, new_line);
error1:
	return;
}

static void add(Ex **e__, cc__PreProcessor *self, String *in)
{
	F("add");
	int32 ol;
	int32 nl;

	ol = self->size;

	Buffer__add(e__, (Buffer*)self, in);
	CHECK(error);
	nl = self->size;

	if (self->cstr_char_alloced <= nl) {
		resize_cstr_char(e__, self, ol, nl);
		CHECK(error);
	}
error:
	return;
}

/**
 * 5.2.1.1 Trigraph sequence
 */
static int32 convert_trigraph(utf8 *cstr, int32 size, uint32 *char_pos,
	uint32 *char_line, uint32 line)
{
	int32 s = 0;
	int32 i = 0;

	while (i < size) {
		char_line[s] = line;
		if (cstr[i] == '?' && cstr[i + 1] == '?') {
			switch (cstr[i + 2]) {
			case '(':
				cstr[s] = '[';
				break;
			case '/':
				cstr[s] = '\\';
				break;
			case ')':
				cstr[s] = ']';
				break;
			case '\'':
				cstr[s] = '^';
				break;
			case '<':
				cstr[s] = '{';
				break;
			case '!':
				cstr[s] = '|';
				break;
			case '>':
				cstr[s] = '}';
				break;
			case '-':
				cstr[s] = '~';
				break;
			case '=':
				cstr[s] = '#';
				break;
			default:
				cstr[s] = '?';
				i -= 2;
			}
			i += 3;
			s++;
		} else {
			cstr[s] = cstr[i];
			s++;
			i++;
		}
		char_pos[s - 1] = (uint32)i - 1;
	}
	cstr[s] = 0;
	return s;
}

/**
 * 5.1.1.2 Translation phases
 */
static void process_file(Ex **e__, cc__PreProcessor *self, String *file)
{
	F("process_file");
	io__File *f;

	f = self->env->include->_->open_file(e__, self->env->include, file);
	CHECK(error1);
	if (f == 0) return;

	self->env->include->_->set_current_file(e__, self->env->include, file,
		self->cstr_char_line[0]);
	CHECK(error2);
	self->env->include->files->_->push(e__, self->env->include->files, f);
	CHECK(error2);
	self->in_comment = 0;
	self->f = f;
	self->offset = 0;	
	return;
error2:
	delete_(f);	
error1:
	return;
}

static int8 process_loop(Ex **e__, cc__PreProcessor *self)
{
	F("process_loop");
	io__File *f;
	uint32 file_line;
	int32 offset = self->offset;

	f = self->f;
	if (f->_->end_of_file(e__, f) == 0) {
		int32 size;

		CHECK(error3);
		f->_->read_line(e__, f, (String*)self);
		CHECK(error3);
		file_line = f->_->get_line_number(e__, f);
		CHECK(error3);

		// 5.1.1.2 Translation phases 1) 
		size = convert_trigraph(self->cstr + offset, 
				self->size - offset,
				self->cstr_char_pos + offset,
				self->cstr_char_line + offset,
				file_line);
		size += offset;
		self->size = size;

		if (size > 0) {
			utf8 *cstr = self->cstr;
			int32 x = size - 1;
			if (x > 0 && (cstr[x] == '\n' || cstr[x] == '\r')) {
				// 5.1.1.2 Translation phases 2)
				// backslash is the last character
				// before the newline then the newline and 
				// \ are deleted
				x--;
				if ((cstr[x] == '\n' || cstr[x] == '\r') &&
					cstr[x] != cstr[x + 1]) 
				{
					x--;
				}
				if (x >= 0 && cstr[x] == '\\') {
					self->size = x;
					cstr[x] = 0;
					offset = x;
				} else {
					cc__PreProcessor__process(e__, self);
					CHECK(error3);
					offset = 0;
				}
			} else {
				cc__PreProcessor__process(e__, self);
				CHECK(error3);
				offset = 0;
			}
		}
		self->offset = offset;
		return 0;
	}

	f->_->close(e__, f);
	CHECK(error3);
	

	if (self->env->include->nb_condition != 0) {
		THROW(-100001, "Missing matching #endif.");
		CC__WARN("", self->env->include->current_file_id, 
			self->env->last_ifelif_line, 
			self->env->last_ifelif_char);
		CHECK(error1);
	}


	self->env->include->files->_->pop(e__, 
		self->env->include->files, (void**)&f);
	CHECK(error1);
	if (f != 0) {
		io__File *k;

		self->env->include->_->revert_file(e__, self->env->include);
		CHECK(error2);
		self->env->include->files->_->pop(e__, 
			self->env->include->files, (void**)&k);
		self->env->include->files->_->push(e__, 
			self->env->include->files, k);
		self->f = k;
		delete_(f);
		if (k == 0) {
			self->_->process(e__, self);
			return 1;
		}
	} else {
		self->f = 0;
		return 1;
	}
	return 0;
error3:
	
error2:
	delete_(f);	
error1:
	return 1;
}


static void get_pos(Ex **e__, cc__PreProcessor *self, utf8 *name, uint32 *file,
	uint32 *line, uint32 *char_pos)
{
	F("get_pos");
	int32 offset;
	int32 csize;
	utf8 *cstr;

	*file = self->env->include->current_file_id;
	CHECK(error);

	cstr = self->cstr;
	CHECK(error);
	csize = self->size;
	CHECK(error);

	offset = (int32)(name - cstr);
	if (offset >= 0 && offset < csize) {
		*line = self->cstr_char_line[offset];
		*char_pos = self->cstr_char_pos[offset];
	}
error:
	;
}

/*
static void dump(Ex **e__, cc__PreProcessor *self)
{
	cc__PreToken *t;

	t = self->tokens->first;
	while (t != 0) {
		ECHO(t->identifier);
		t = t->next;
	}
}
*/

static void push_token(Ex **e__, cc__PreProcessor *self, utf8 *name, 
		int32 size, uint32 type)
{
	F("push_token");
	uint32 line;
	uint32 char_pos;
	uint32 file;

	get_pos(e__, self, name, &file, &line, &char_pos); 	
	cc__PreTokens__push(e__, self->tokens, name, size, type,
		file, line, char_pos);
	///ECHO(self->tokens->last->identifier);
	CHECK(error);
	return;
error:
	return;
}


/**
 * 6.4.9 Comments
 */
static void push_comment(Ex **e__, cc__PreProcessor *self, 
	utf8 *cstr, int32 size)
{
	F("push_comment");

	(void)size;

	push_token(e__, self, cstr, 0, CC_COMMENT);
	CHECK(error);
error:
	return;
}

/**
 * 6.4.9 Comments
 */
static void push_comment_line(Ex **e__, cc__PreProcessor *self,
	utf8 *cstr, int32 size)
{
	F("push_comment_line");

	(void)size;

	push_token(e__, self, cstr, 0, CC_LINE_COMMENT);
	CHECK(error);
error:
	return;
}

/**
 * 6.4.4.4 Character constants
 */
static void push_character_constant(Ex **e__, cc__PreProcessor *self,
	utf8 *cstr, int32 size)
{
	F("push_character_constant");

	push_token(e__, self, cstr, size, CC_CHARACTER);
	CHECK(error);
error:
	return;
}

/**
 * 6.4.5 String literals
 */
static void push_string_literal(Ex **e__, cc__PreProcessor *self,
	utf8 *cstr, int32 size)
{
	F("push_string_literal");

	push_token(e__, self, cstr, size, CC_STRING);
	CHECK(error);
error:
	return;
}

// 6.4.6 Punctuator
static void push_punctuator(Ex **e__, cc__PreProcessor *self, 
	utf8 *cstr, int32 size)
{
	F("push_punctuator");
	uint32 value = 0;

	if (size == 1) {
		value = (uint32)cstr[0];
	} else if (size == 2) {
		value = (uint32)cstr[0] | ((uint32)cstr[1] << 8);
	} else if (size == 3) {
		value = (uint32)cstr[0] | ((uint32)cstr[1] << 8)
			| ((uint32)cstr[2] << 16);
	} else {
		value = (uint32)cstr[0] | ((uint32)cstr[1] << 8)
			| ((uint32)cstr[2] << 16) | ((uint32)cstr[3] << 24);
	}
	push_token(e__, self, cstr, size, value);
	CHECK(error);
error:
	return;
}

/**
 * White space
 */ 
static void push_white_space(Ex **e__, cc__PreProcessor *self,
	utf8 *cstr, int32 size)
{
	F("push_white_space");

	push_token(e__, self, cstr, size, CC_WHITE_SPACE);
	CHECK(error);
error:
	return;
}

/**
 * identifier
 */ 
static void push_identifier(Ex **e__, cc__PreProcessor *self,
	utf8 *cstr, int32 size)
{
	F("push_identifier");

	push_token(e__, self, cstr, size, CC_IDENTIFIER);
	CHECK(error);
error:
	return;
}

static void push_invalid(Ex **e__, cc__PreProcessor *self,
	utf8 *cstr, int32 size)
{
	F("push_invalid");

	push_token(e__, self, cstr, size, CC_INVALID_CHAR);
	CHECK(error);
error:
	return;
}


/**
 * Number
 */ 
static void push_number(Ex **e__, cc__PreProcessor *self,
	utf8 *cstr, int32 size, uint32 type)
{
	F("push_number");

	push_token(e__, self, cstr, size, type);
	CHECK(error); 
error:
	return;
}


static void tokenize_number(Ex **e__, cc__PreProcessor *self,
	utf8 *cstr, int32 size, int32 *i_, utf8 c, int32 *s_, uint32 *type_)
{
	int32 i = *i_;
	int32 s = i;
	uint32 type = CC_NUMBER;

	(void)e__;
	(void)self;
	(void)size;

	if (c == '0') {
		i++;
		c = cstr[i];
		if (c == 'x' || c == 'X') {
			type |= CC_HEXADECIMAL;
			i++;
			c = cstr[i];
			while (c == '.' || 
				( ((type & CC_DOUBLE) == CC_DOUBLE)
					&& 
					(c == 'p' ||
					 c == 'P' ||
					 c == '+' ||
					 c == '-')) ||
				(c >= '0' && c <= '9') ||
				(c >= 'a' && c <= 'f') ||
				(c >= 'F' && c <= 'F')) 
			{
				if (c == '.') {
					type |= CC_DOUBLE;
				}
				i++;
				c = cstr[i];
			}
		} else {
			type |= CC_OCTAL;
			while (c >= '0' && c <= '7') {
				i++;
				c = cstr[i];
			}
		}
	} else {
		type |= CC_DECIMAL;
		while (c == '.' ||
			(((type & CC_DOUBLE) == CC_DOUBLE) && 
				(c == 'e' ||
				c == 'E' ||
				c == '+' ||
				c == '-')) ||
			(c >= '0' && c <= '9')) 
		{
			if (c == '.') {
				type |= CC_DOUBLE;
			}
			i++;
			c = cstr[i];
		}
	}
	if ((type & CC_DOUBLE) == CC_DOUBLE) {
		if (c == 'l' || c == 'L') {
			i++;
			type |= CC_LONG_DOUBLE;
		} else if (c == 'f' || c == 'F') {
			i++;
			type |= CC_FLOAT;
		}	
	} else {
		if (c == 'u' || c == 'U') {
			type |= CC_UNSIGNED;
			i++;
			c = cstr[i];
			if (c == 'l' || c == 'L') {
				type |= CC_LONG;
				i++;
				c = cstr[i];
				if (c == 'l' || c == 'L') {
					i++;
					type |= CC_LONG_LONG;
				}
			}
		} else if (c == 'l' || c == 'L') {
			type |= CC_LONG;
			i++;
			c = cstr[i];
			if (c == 'l' || c == 'L') {
				type |= CC_LONG_LONG;
				i++;
				c = cstr[i];
			}
			if (c == 'u' || c == 'U') {
				type |= CC_UNSIGNED;
				i++;
			}
		}
	}
	while (c == '_' || c >= 0x80 ||
		(c >= 'a' && c <= 'z') ||
		(c >= 'A' && c <= 'Z') ||
		(c >= '0' && c <= '9') ||
		c == '$' || c == '@' ||
		c == 0x60)
	{
		i++;
		c = cstr[i];
		type |= CC_INVALID;
	}
	*i_ = i;
	*s_ = s;
	*type_ = type;
	return;
}

/**
 * 5.1.1.2 Translation phases 3)
 * split the string into preprocessing tokens and replace comment by one
 * space character.
 */
static void tokenize_cstr(Ex **e__, cc__PreProcessor *self,
	utf8 *cstr, int32 size)
{
	F("tokenize_cstr");
	utf8 c;
	int32 i = 0;
	uint32 line;
	uint32 char_pos;
	uint32 file;

	//_CrtDbgBreak();
	while (IS_WHITE_SPACE(cstr[i])) i++;

	
	if (self->env->include->condition != 1) {
		if (cstr[i] == '%' && cstr[i + 1] == ':') {
		} else if (cstr[i] == '#') {
		} else {
			int8 in_com = 0;
			size--;
			while (i < size) {	
				if (in_com == 0 && cstr[i] == '/' 
					&& cstr[i + 1] == '*') 
				{
					i++;
					in_com = 1;
				} else if (in_com == 1 && cstr[i] == '*' 
					&& cstr[i + 1] == '/') 
				{
					i++;
					in_com = 0;
				}
				i++;
			}
			self->in_comment = in_com;
			self->size = 0;
			return;
		}
	}

	if (i != 0) {
		push_white_space(e__, self, cstr, i);
		CHECK(error);
	}
	
	while (i < size) {
		int32 j;

		c = cstr[i];	
		if (c == '"') {
			int8 esc = 0;
			// 6.4.5 String literals
			j = i + 1;
			while (j < size && (cstr[j] != '"' || esc == 1))
			{
				if (cstr[j] == '\\') {
					if (esc == 1) {
						esc = 0;
					} else {
						esc = 1;
					}
				} else {
					esc = 0;
				}
				j++;
			}
			if (j == size) {
				get_pos(e__, self, cstr + j - 1, 
					&file, &line, &char_pos); 	

				THROW(-100002, "Newline termination "
					"in string literal");
				CC__WARN("", file, line, char_pos);
				CHECK(error);
			}
			push_string_literal(e__, self, cstr + i + 1, j - i - 1);
			CHECK(error);
			i = j;
		} else if (c == '\'') {
			int8 esc = 0;
			// 6.4.4.4 Character constants
			j = i + 1;
			while (j < size && !(cstr[j] == '\'' && esc == 0)) {
				if (cstr[j] == '\\') {
					if (esc == 1) {
						esc = 0;
					} else {
						esc = 1;
					}
				} else {
					esc = 0;
				}
				j++;
			}
			if (j == size) {
				// unexpected new line
				get_pos(e__, self, cstr + j - 1, 
					&file, &line, &char_pos); 
				THROW(-100003, "Newline termination "
					"in character constant");	
				CC__WARN("", file, line, char_pos);
				CHECK(error);
			}
			push_character_constant(e__, self,
				cstr + i + 1, j - i - 1);
			CHECK(error);
			i = j;
		} else if (c == '/') {
			// 6.4.9 Comments
			c = cstr[i + 1];
			if (c == '/') {
				// //
				i += 2;
				push_comment_line(e__, self,
					cstr + i, size - i);
				CHECK(error);
				self->size = 0;
				return;
			} else if (c == '*') {
				// /*
				j = i + 2;
				i += 2;
				while (j < size - 1 && !(cstr[j] == '*' && 
					cstr[j + 1] == '/'))
				{
					j++;
				}
				if (j == size - 1) {
					// this comment doesn't end 
					// on this line
					int32 k;
					utf8 *ptr = self->cstr;
			
					j = i;
					k = 0;
					while (k < size - i) {
						ptr[k] = cstr[j];
						k++;
						j++;
					}
					ptr[k] = 0;
					self->size = k;
					self->in_comment = 1;
					return;
				} else {
					push_comment(e__, self, 
						cstr + i, j - i);
					CHECK(error); 
					i = j + 2;
				}
				i--;
			} else if (c == '=') {
				// /=
				push_punctuator(e__, self, cstr + i, 2);
				CHECK(error);
				i++;
			} else {
				// /
				push_punctuator(e__, self, cstr + i, 1); 
				CHECK(error);
			}
		} else if (c == '#') {
			c = cstr[i + 1];
			if (c == '#') {
				// ##
				push_punctuator(e__, self, cstr + i, 2);
				CHECK(error);
				i++;
			} else {
				// #
				push_punctuator(e__, self, cstr + i, 1);
				CHECK(error);
				self->has_sharp = 1;
			}
		} else if (c == '%') {
			c = cstr[i + 1];
			if (c == ':') {
				// 6.4.6 "%:" is an alternative for "#"
				if (cstr[i + 2] == '%' && cstr[i + 3] == ':') {
					// %:%:
					push_punctuator(e__, self, cstr + i, 4);
					CHECK(error);
					i += 3;
				} else {
					// %:
					push_punctuator(e__, self, cstr + i, 2);
					CHECK(error);
					self->has_sharp = 1;
					i++;
				}
			} else if (c == '>') {
				// %>
				push_punctuator(e__, self, cstr + i, 2);
				CHECK(error);
				i++;
			} else if (c == '=') {
				// %=
				push_punctuator(e__, self, cstr + i, 2);
				CHECK(error);
				i++;
			} else {
				// %
				push_punctuator(e__, self, cstr + i, 1);
				CHECK(error);
			}
		} else if (c == '_' || c >= 0x80 ||
				(c >= 'a' && c <= 'z') ||
				(c >= 'A' && c <= 'Z') ||
				c == '$' || c == '@' ||
				c == 0x60 || c == '\\')
		{
			// 6.4.2 Identifiers
			int32 s = i;

			i++;
			c = cstr[i];
			while (c == '_' || c >= 0x80 ||
				(c >= 'a' && c <= 'z') ||
				(c >= 'A' && c <= 'Z') ||
				(c >= '0' && c <= '9') ||
				c == '$' || c == '@' ||
				c == 0x60 || c == '\\')
			{
				i++;
				c = cstr[i];
			}
			push_identifier(e__, self, cstr + s, i - s);
			CHECK(error);
			i--;
		} else if (c == '.' && cstr[i + 1] == '.' &&
			cstr[i + 2] == '.')
		{
			// ...
			push_punctuator(e__, self, cstr + i, 3);
			CHECK(error);
			i += 2;
		} else if (c == '.' && !(cstr[i + 1] >= '0' &&
			cstr[i + 1] <= '9'))
		{
			// .
			push_punctuator(e__, self, cstr + i, 1);
			CHECK(error);
 		} else if ((c >= '0' && c <= '9') || c == '.') {
			// 6.4.4.1 Integer constants
			// 6.4.4.2 Floating constants
			uint32 type;
			int32 s;

			tokenize_number(e__, self, cstr, size, 
				&i, c, &s, &type);
			CHECK(error);
			push_number(e__, self, cstr + s, i - s, type);
			CHECK(error);
			i--;
		} else if (c == '<') {
			c = cstr[i + 1];
			if (c == '<') {
				if (cstr[i + 2] == '=') {
					// <<=
					push_punctuator(e__, self, cstr + i, 3);
					CHECK(error);
					i += 2;
				} else {
					// <<
					push_punctuator(e__, self, cstr + i, 2);
					CHECK(error);
					i++;
				}
			} else if (c == '=') {
				// <=
				push_punctuator(e__, self, cstr + i, 2);
				CHECK(error);
				i++;
			} else if (c ==':') {
				// <:
				push_punctuator(e__, self, cstr + i, 2);
				CHECK(error);
				i++;
			} else if (c == '%') {
				// <%
				push_punctuator(e__, self, cstr + i, 2);
				CHECK(error);
				i++;
			} else {
				// <
				push_punctuator(e__, self, cstr + i, 1);
				CHECK(error);
			}						
		} else if (c == '>') {
			c = cstr[i + 1];
			if (c == '>') {
				if (cstr[i + 2] == '=') {
					// >>=
					push_punctuator(e__, self, cstr + i, 3);
					CHECK(error);
					i += 2;
				} else {
					// >>
					push_punctuator(e__, self, cstr + i, 2);
					CHECK(error);
					i++;
				}
			} else if (c == '=') {
				// >=
				push_punctuator(e__, self, cstr + i, 2);
				CHECK(error);
				i++;
			} else {
				// >
				push_punctuator(e__, self, cstr + i, 1);
				CHECK(error);
			}			
		} else if (c == '+') {
			c = cstr[i + 1];
			if (c == '+') {
				// ++
				push_punctuator(e__, self, cstr + i, 2);
				CHECK(error);
				i++;
			} else if (c == '=') {
				// +=
				push_punctuator(e__, self, cstr + i, 2);
				CHECK(error);
				i++;
			} else {
				// +
				push_punctuator(e__, self, cstr + i, 1);
				CHECK(error);
			}
		} else if (c == '-') {
			c = cstr[i + 1];
			if (c == '-') {
				// --
				push_punctuator(e__, self, cstr + i, 2);
				CHECK(error);
				i++;
			} else if (c == '=') {
				// -=
				push_punctuator(e__, self, cstr + i, 2);
				CHECK(error);
				i++;
			} else if (c == '>') {
				// ->
				push_punctuator(e__, self, cstr + i, 2);
				CHECK(error);
				i++;
			} else {
				// -
				push_punctuator(e__, self, cstr + i, 1);
				CHECK(error);
			}
		} else if (c == '=') {
			if (cstr[i + 1] == '=') {
				// ==
				push_punctuator(e__, self, cstr + i, 2);
				CHECK(error);
				i++;
			} else {
				// =
				push_punctuator(e__, self, cstr + i, 1);
				CHECK(error);
			}			
		} else if (c == '!') {
			if (cstr[i + 1] == '=') {
				// !=
				push_punctuator(e__, self, cstr + i, 2);
				CHECK(error);
				i++;
			} else {
				// !
				push_punctuator(e__, self, cstr + i, 1);
				CHECK(error);
			}
		} else if (c == '&') {
			c = cstr[i + 1];
			if (c == '=') {
				// &=
				push_punctuator(e__, self, cstr + i, 2);
				CHECK(error);
				i++;
			} else if (c == '&') {
				// &&
				push_punctuator(e__, self, cstr + i, 2);
				CHECK(error);
				i++;
			} else {
				// &
				push_punctuator(e__, self, cstr + i, 1); 
				CHECK(error);
			}
		} else if (c == '|') {
			c = cstr[i + 1];
			if (c == '=') {
				// |=
				push_punctuator(e__, self, cstr + i, 2);
				CHECK(error);
				i++;
			} else if (c == '|') {
				// ||
				push_punctuator(e__, self, cstr + i, 2);
				CHECK(error);
				i++;
			} else {
				// |
				push_punctuator(e__, self, cstr + i, 1);
				CHECK(error);
			}
		} else if (c == '*') {
			if (cstr[i + 1] == '=') {
				// *=
				push_punctuator(e__, self, cstr + i, 2);
				CHECK(error);
				i++;
			} else {
				// *
				push_punctuator(e__, self, cstr + i, 1);
				CHECK(error);
			}
		} else if (c == '/') {
			if (cstr[i + 1] == '=') {
				// /=
				push_punctuator(e__, self, cstr + i, 2);
				CHECK(error);
				i++;
			} else {
				// /
				push_punctuator(e__, self, cstr + i, 1);
				CHECK(error);
			}
		} else if (c == '^') {
			if (cstr[i + 1] == '=') {
				// ^=
				push_punctuator(e__, self, cstr + i, 2);
				CHECK(error);
				i++;
			} else {
				// ^
				push_punctuator(e__, self, cstr + i, 1);
				CHECK(error);
			}
		} else if (c == ':') {
			if (cstr[i + 1] == '>') {
				// :>
				push_punctuator(e__, self, cstr + i, 2);
				CHECK(error);
				i++;
			} else {
				// :
				push_punctuator(e__, self, cstr + i, 1);
				CHECK(error);
			}
		} else if ((c >= '!' && c <= '/') ||
			(c >= ':' && c <= '@') ||
			(c >= '[' && c <= '^') ||
			(c >= '{' && c <= '~'))
		{
			// other punctuator...
			push_punctuator(e__, self, cstr + i, 1);
			CHECK(error);
		} else {
			// other character...
			push_invalid(e__, self, cstr + i, 1);
			CHECK(error);
		}
		i++;
		j = i;
		while (IS_WHITE_SPACE(cstr[i])) i++;
		if (i != j) {
			push_white_space(e__, self, cstr + j, i - j);
			CHECK(error);
		}
	}

	if (self->in_comment == 0) {
		self->size = 0;
	}
	return;
error:
	return;
}

static uint32 get_type_of_cstr(Ex **e__, cc__PreProcessor *self,
	utf8 *cstr, int32 size)
{
	F("get_type_of_cstr");
	uint32 type = CC_INVALID;
	cc__PreProcessor *pp;
	cc__PreToken *n;

	pp = new_(cc__PreProcessor);
	CHECK(error1);
	pp->cstr = cstr;
	pp->size = size;
	pp->env = self->env;
	tokenize_cstr(e__, pp, cstr, size);
	pp->env = 0;
	pp->cstr = 0;
	pp->size = 0;
	CHECK(error2); 
	n = pp->tokens->first;
	if (n != 0) {
		type = n->type;
		n = n->next;
		if (n != 0) {
			type |= CC_INVALID;
		}

		// concatenated # and ## are not preprocess operator
		if (type == P('#', '#', 0, 0) ||
			type == P('#', 0, 0, 0))
		{
			type |= CC_INVALID;
		}
	}
	delete_(pp);
	CHECK(error1);
	return type;
error2:
	delete_(pp);
error1:
	return 0;
}


/**
 * expand the source tokens if they are macros.
 */
static void expand_macros(Ex **e__, cc__PreProcessor *self,
	cc__PreTokens *tokens)
{
	F("expand_macros");

	self->macros->_->expand_macros(e__, self->macros, tokens);
	CHECK(error);
	return;
error:
	return;
}

/**
 * transform "defined(macro)" into "1" or "0" and white spaces
 */
static cc__PreToken *set_defined(Ex  **e__, cc__PreProcessor *self,  
	cc__PreToken *start)
{
	F("set_defined");
	int8 ret = 0;
	int32 par = 0;
	cc__PreToken *first = start;
	cc__PreToken *t = start;
	
	start = start->next;

	while (start != 0 &&
		((start->type & CC_WHITE_SPACE) == CC_WHITE_SPACE ||
		start->type == P('(', 0, 0, 0))) 
	{
		if (start->type == P('(', 0, 0, 0)) {
			par++;
		}
		start->type = CC_WHITE_SPACE;
		start = start->next;
	}

	if (start == 0 || (start->type & CC_IDENTIFIER) != CC_IDENTIFIER) {
		t = start;
		if (t == 0) t = first;
		// expecting identifier
		THROW(-100010, "Expecting identifier");
		CC__WARN("", t->file, t->line, t->char_pos);
		CHECK(error);
	}
	if (self->macros->_->is_defined(
		e__, self->macros, start->identifier) != 0)
	{
		ret = 1;
	}
	CHECK(error);
	start->type = CC_WHITE_SPACE;
	start = start->next;

	while (start != 0 &&
		((start->type & CC_WHITE_SPACE) == CC_WHITE_SPACE ||
		(par > 0 && start->type == P(')', 0, 0, 0)))) 
	{
		t = start;
		if (start->type == P(')', 0, 0, 0)) {
			par--;
		}
		start->type = CC_WHITE_SPACE;
		start = start->next;
	}
	
	if (par != 0) {
		// expecting )
		THROW(-100011, "Expecting ')'");
		CC__WARN("", t->file, t->line, t->char_pos);
		CHECK(error);
	}
	
	if (ret != 0) {
		first->type = CC_DECIMAL;
		first->identifier[0] = '1';
	} else {
		first->type = CC_DECIMAL;
		first->identifier[0] = '0';
	}
 	first->identifier[1] = 0;
	return start;
error:
	return 0;
}

static int8 boolean_solver(Ex **e__, cc__PreProcessor *self, 
	cc__PreToken *start)
{
	F("boolean_slover");
	int8 ret;
	cc__PreToken *n;
	cc__PreToken *expand;
	cc__PreExpression *exp;

	exp = new_(cc__PreExpression);
	CHECK(error1);
	exp->_->set_env(e__, exp, self->env);
	CHECK(error2);
	n = start;
	while (n) {
		if ((n->type & CC_IDENTIFIER) == CC_IDENTIFIER) {
			utf8 *p1;
			utf8 *p2;
			p1 = (utf8*)"defined";
			p2 = n->identifier;
			
			while (*p1 != 0 && *p2 == *p1) {
				p1++;
				p2++;
			}
			if (*p1 == *p2) {
				n = set_defined(e__, self, n);
				CHECK(error2);
			}
		}
		if (n) n = n->next;
	}

	// expand tokens after the #if and keep the global next_to_expand!
	expand = self->tokens->next_to_expand_token;
	n = self->tokens->next_to_expand_token = start->previous;
	expand_macros(e__, self, self->tokens);
	CHECK(error2);
	self->tokens->next_to_expand_token = expand;

	n = n->next;
	while (n) {
		uint32 file;
		uint32 line;
		uint32 char_pos;

		self->tokens->_->get_pos(e__, self->tokens, n, &file,
					&line, &char_pos);
		CHECK(error2);							
		if ((n->type & CC_IDENTIFIER) == CC_IDENTIFIER) {
			exp->_->push_number(e__, exp, (utf8*)"0", CC_DECIMAL,
				file, line, char_pos);
			CHECK(error2);
		} else if ((n->type & CC_NUMBER) == CC_NUMBER || 
			n->type == CC_CHARACTER) 
		{
			exp->_->push_number(e__, exp, n->identifier, n->type,
				file, line, char_pos);
			CHECK(error2);
		} else if ((n->type & CC_ISNOT_PUNCTUATOR) == 0) {
			exp->_->push_punctuator(e__, exp, n->type,
				file, line, char_pos);
			CHECK(error2);
		} else if ((n->type & CC_WHITE_SPACE) != CC_WHITE_SPACE) {
			// not evalutated as integer
			THROW(-100012, "preprocessing expression cannot be "
				"evalutated as an integer");
			CC__WARN("", n->file, n->line, n->char_pos);
			CHECK(error2);
		}
		if (n) n = n->next;
	}

	ret = exp->_->solve(e__, exp);
	CHECK(error2);
	delete_(exp);
	CHECK(error1);
	return ret;
error2:
	delete_(exp);
error1:
	return 0;
}


static int8 defined_solver(Ex **e__, cc__PreProcessor *self,
	cc__PreToken *start)
{
	F("defined_solver");
	int8 ret = 0;
	int32 par = 0;
	cc__PreToken *t = start;

	start = start->next;
	while (start != 0 &&
		((start->type & CC_WHITE_SPACE) == CC_WHITE_SPACE ||
		start->type == P('(', 0, 0, 0))) 
	{
		t = start;
		if (start->type == P('(', 0, 0, 0)) {
			par++;
		}
		start = start->next;
	}

	if (start == 0 || (start->type & CC_IDENTIFIER) != CC_IDENTIFIER) {
		// Expecting identifier
		THROW(-100013, "Expecting identifier");
		CC__WARN("", t->file, t->line, t->char_pos);
		CHECK(error);
	}

	if (self->macros->_->is_defined(
		e__, self->macros, start->identifier))
	{
		ret = 1;
	}
	CHECK(error);
	start = start->next;
	
	while (start != 0 &&
		((start->type & CC_WHITE_SPACE) == CC_WHITE_SPACE ||
		start->type == P(')', 0, 0, 0))) 
	{
		t = start;
		if (start->type == P(')', 0, 0, 0)) {
			par--;
		}
		start = start->next;
	}
	
	if (par != 0) {
		THROW(-100014, "Expecting ')'");
		CC__WARN("", t->file, t->line, t->char_pos);
		CHECK(error);
	}
	
	return ret;
error:
	return 0;
}


static void include_file(Ex **e__, cc__PreProcessor *self, String *name,
	uint32 file, uint32 line, uint32 char_pos, String *current)
{
	F("include_file");
	String *s;

	s = new_(String);
	CHECK(error1);
	self->env->include->_->get_include_file(e__, self->env->include,
		name, s, current);
	CHECK(error2);

	if (s->size < 1) {
		THROW(-100020, "Cannot open include file");
		CC__WARN("", file, line, char_pos);
		CHECK(error2);
	}
	self->_->process_file(e__, self, s);
	CHECK(error2);
	delete_(s);
	return;
error2:
	delete_(s);
error1:
	return;
}

static void include_directive_local(Ex **e__, cc__PreProcessor *self,
	cc__PreToken *start, cc__PreToken *begin)
{
	F("include_directive_local");
	uint32 file, line, char_pos;
	String *c_path;
	String *s;		
	int32 i;
	utf8 *p;
	String h;
	
	STR(h, start->identifier);
	start->identifier =  Gc__malloc(e__, 1);
	CHECK(error1);
	start->identifier[0] = 0;

	s = new_(String);
	CHECK(error1);
       	c_path = self->env->include->_->get_current_file_full_path(e__,
		self->env->include);
	CHECK(error2);
	s->_->add(e__, s, c_path);
	CHECK(error2);
	p = s->_->get_cstr(e__, s);
	CHECK(error2);
	i = s->_->get_size(e__, s);
	CHECK(error2);
	while (i > 0) {
		i--;
		if (p[i] == '\\' || p[i] == '/') {
			break;
		}
	}
	if (i == 0) {
		p[0] = '.';
		i++;
	} 
	p[i] = 0;	
	s->size = i;
	file = start->file;
	line = start->line;
	char_pos = start->char_pos;

	// remove the directive tokens!
	self->tokens->_->delete_end(e__, self->tokens, begin);
	CHECK(error2);

	include_file(e__, self, &h, file, line, char_pos, s);
	CHECK(error2);
			
	delete_(s);
	CHECK(error1);
	return;
error2:
	delete_(s);
error1:
	return;
}

static void include_directive_standard(Ex **e__, cc__PreProcessor *self,
	cc__PreToken *start, cc__PreToken *begin)
{
	F("include_directive_standard");
	String *s;
	uint32 file, line, char_pos;

	s = new_(String);
	CHECK(error1);
	while (start->next && 
		start->next->type != P('>', 0, 0, 0))
	{
		start = start->next;
		if ((start->type & CC_STRING) == CC_STRING) {
			String str;
			String gu;

			STR(gu, "\"");
			STR(str, start->identifier);
			s->_->add(e__, s, &gu);
			CHECK(error2);
			s->_->add(e__, s, &str);
			CHECK(error2);
			s->_->add(e__, s, &gu);
			CHECK(error2);
		} else if ((start->type & CC_CHARACTER) == CC_CHARACTER) {
			String str;
			String gu;

			STR(gu, "'");
			STR(str, start->identifier);
			s->_->add(e__, s, &gu);
			CHECK(error2);
			s->_->add(e__, s, &str);
			CHECK(error2);
			s->_->add(e__, s, &gu);
			CHECK(error2);
		} else {
			String str;

			STR(str, start->identifier);
			s->_->add(e__, s, &str);
			CHECK(error2);
		}
	}
	if (start->next == 0 || 
		start->next->type != P('>', 0, 0, 0))
	{
		int32 l = 0;
		while (start->identifier[l]) l++;
		THROW(-100016, "Expecting '>' after file name.");
		CC__WARN("", start->file, start->line, start->char_pos + l);
		CHECK(error2);
	}
	file = start->file;
	line = start->line;
	char_pos = start->char_pos;

	// remove the directive tokens!
	self->tokens->_->delete_end(e__, self->tokens, begin);
	CHECK(error2);
	include_file(e__, self, s, file, line, char_pos, 0);
	CHECK(error2);
	delete_(s);
	CHECK(error1);
	return;
error2:
	delete_(s);
error1:
	return;
}

static void include_directive(Ex **e__, cc__PreProcessor *self,
	cc__PreTokens *tokens, cc__PreToken *start)
{
	F("include_directive");
	cc__PreToken *begin = start;
	cc__PreToken *t = start;

	while (begin->type != P('#', 0, 0, 0) &&
		begin->type != P('%', ':', 0, 0))
	{
		begin = begin->previous;	
	}

	if (self->env->include->condition != 1) {
		// remove the directive tokens!
		self->tokens->_->delete_end(e__, self->tokens, begin);
		CHECK(error);
		return;
	}

	if (start->next == 0) {
		int32 l = 0;
		while (t->identifier[l]) l++;
		THROW(-100018, "Missing #include file name");
		CC__WARN("", t->file, t->line, t->char_pos + l);
		CHECK(error);
	}
	start = start->next;
	t = start;

	while (start && (start->type & CC_WHITE_SPACE) == CC_WHITE_SPACE) {
		t = start;
		start = start->next;
	}
	
	if (start == 0) {
		THROW(-100017, "Missing include file name");
		CC__WARN("", t->file, t->line, t->char_pos);
		CHECK(error);
	}

	// expand the tokens after #include <
	if ((start->type & CC_STRING) != CC_STRING && 
		start->type != P('<', 0, 0, 0)) 
	{
		cc__PreToken *l;
		cc__PreToken *t = tokens->next_to_expand_token;
	
		tokens->next_to_expand_token = start->previous;
		l = start->previous;
		expand_macros(e__, self, tokens);
		CHECK(error);
		tokens->next_to_expand_token = t;
		start = l->next;
	}

	while (start && (start->type & CC_WHITE_SPACE) == CC_WHITE_SPACE) {
		t = start;
		start = start->next;
	}
	
	self->size = 0;

	if (start == 0 || ((start->type & CC_STRING) != CC_STRING && 
		start->type != P('<', 0, 0, 0)))
	{
		THROW(-100019, "Expecting '<' or '\"' after #include");
		CC__WARN("", t->file, t->line, t->char_pos);
		CHECK(error);
	}

	if ((start->type & CC_STRING) == CC_STRING) {
		include_directive_local(e__, self, start, begin);
		CHECK(error);
	} else {
		include_directive_standard(e__, self, start, begin);
		CHECK(error);
	}
	return;
error:
	return;
}

/**
 * push the current "condition" on the stack and set the condition to "value".
 * "condition" is the value used to either skip some part of the file or to
 * process them.
 */
static void push_condition(Ex **e__, cc__PreProcessor *self, int8 value)
{
	F("push_condition");

	self->env->include->_->push_condition(e__, self->env->include, value);
	CHECK(error);
	return;
error:
	return;
}

/**
 * change the value of the current "condition"
 */ 
static void set_condition(Ex **e__, cc__PreProcessor *self, int8 value,
	cc__PreToken *t)
{
	F("set_condition");

	if (self->env->include->nb_condition == 0) {
		THROW(15, "C compiler internal error.");
		CC__WARN("", t->file, t->line, t->char_pos);
		CHECK(error);
	}
	self->env->include->condition = value;
error:
	return;
}

/**
 * get the value of the current condition
 */
static int8 get_condition(Ex **e__, cc__PreProcessor *self, cc__PreToken *t)
{
	F("get_condition");

	if (self->env->include->nb_condition == 0) {
		THROW(-100021, "#elif without #if, #ifdef nor #ifndef");
		CC__WARN("", t->file, t->line, t->char_pos);
		CHECK(error);
	}
	return self->env->include->condition;
error:
	return 0;
}

/**
 * pop the previous "condition" form the stack.
 */
static void pop_condition(Ex **e__, cc__PreProcessor *self, cc__PreToken *t)
{
	F("pop_condition");

	if (self->env->include->nb_condition == 0) {
		THROW(-100022, "#endif without #if, #ifdef nor #ifndef");
		CC__WARN("", t->file, t->line, t->char_pos);
		CHECK(error);
	}
	self->env->include->_->pop_condition(e__, self->env->include);
	CHECK(error);
	return;
error:
	return;
}

static void set_last_ifelif(Ex **e__, cc__PreProcessor *self, 
	cc__PreToken *start)
{
	F("set_last_ifelif");

	(void)__func__;
	(void)e__;

	self->env->last_ifelif_line = start->line;
	self->env->last_ifelif_char = start->char_pos;
	return;
}

/**
 * self->condition == 0, means that the if condition was false.
 * 1, means the condition is valid, so we must process the current body.
 * 2, means in an "if/elif/else" that the condition has already been true in
 * a previous if body, so we must not process the current body.
 *
 * "#if" directive method
 */
static void if_directive(Ex **e__, cc__PreProcessor *self,
	cc__PreToken *start)
{
	F("if_directive");
	int8 value;

	set_last_ifelif(e__, self, start);
	CHECK(error);	

	if (self->env->include->condition != 1) {
		push_condition(e__, self, 2);
		CHECK(error);
		return;
	}

	value = boolean_solver(e__, self, start->next);
	CHECK(error);
	if (value != 0) {
		push_condition(e__, self, 1);
		CHECK(error);
	} else {
		push_condition(e__, self, 0);
		CHECK(error);
	}
error:
	return;
}

/**
 * "#ifdef" method.
 */
static void ifdef_directive(Ex **e__, cc__PreProcessor *self,
	cc__PreToken *start)
{
	F("ifdef_directive");
	int8 value;

	set_last_ifelif(e__, self, start);
	CHECK(error);	
	if (self->env->include->condition != 1) {
		push_condition(e__, self, 2);
		CHECK(error);
		return;
	}

	value = defined_solver(e__, self, start->next);
	CHECK(error);
	if (value != 0) {
		push_condition(e__, self, 1);
		CHECK(error);
	} else {
		push_condition(e__, self, 0);
		CHECK(error);
	}
error:
	return;
}

/**
 * "#ifndef" method.
 */
static void ifndef_directive(Ex **e__, cc__PreProcessor *self,
	cc__PreToken *start)
{
	F("ifndef_directive");
	int8 value;

	set_last_ifelif(e__, self, start);
	CHECK(error);	
	if (self->env->include->condition != 1) {
		push_condition(e__, self, 2);
		CHECK(error);
		return;
	}
	
	value = !defined_solver(e__, self, start);
	CHECK(error);

	if (value != 0) {
		push_condition(e__, self, 1);
		CHECK(error);
	} else {
		push_condition(e__, self, 0);
		CHECK(error);
	}
error:
	return;
}

/**
 * "#elif" method
 */
static void elif_directive(Ex **e__, cc__PreProcessor *self,
	cc__PreToken *start)
{
	F("elif_directive");
	int8 value;

	set_last_ifelif(e__, self, start);
	CHECK(error);		
	value = get_condition(e__, self, start);
	CHECK(error);
	if (value != 0) {
		set_condition(e__, self, 2, start);
		CHECK(error);
		return;
	}

	value = boolean_solver(e__, self, start->next);
	CHECK(error);
	if (value != 0) {
		set_condition(e__, self, 1, start);
		CHECK(error);
	} else {
		set_condition(e__, self, 0, start);
		CHECK(error);
	}
error:
	return;
}

/**
 * "#else" method.
 */
static void else_directive(Ex **e__, cc__PreProcessor *self,
	cc__PreToken *start)
{
	F("else_directive");
	int8 value;

	set_last_ifelif(e__, self, start);
	CHECK(error);	
	value = get_condition(e__, self, start);
	CHECK(error);
	if (value == 0) {
		set_condition(e__, self, 1, start);
		CHECK(error);
	} else {
		set_condition(e__, self, 2, start);
		CHECK(error);
	}
error:
	return;
}

/**
 * "#endif" method.
 */
static void endif_directive(Ex **e__, cc__PreProcessor *self,
	cc__PreToken *start)
{
	F("endif_directive");

	pop_condition(e__, self, start);
	CHECK(error);
error:
	return;
}


/**
 * check if "id" is in the chained list "token"
 */
static int8 is_arg(Ex **e__, cc__PreProcessor *self,
	cc__PreToken *token, utf8 *id, cc__PreToken *start)
{
	F("is_arg");

	(void)__func__;
	(void)e__;
	(void)self;

	while (token && token != start) {
		utf8 *ptr = token->identifier;
		int32 i = 0;

		if (ptr[0] == '.' && ptr[1] == '.' && ptr[2] == '.' &&
			ptr[3] == 0)
		{
			ptr = (utf8*)"__VA_ARGS__";
		}
		while (ptr[i] != 0 && ptr[i] == id[i]) i++;
		if (ptr[i] == id[i]) return 1;

		token = token->next;
	}	
	return 0;
}

static void define_directive(Ex **e__, cc__PreProcessor *self,
	cc__PreTokens *tokens, cc__PreToken *start)
{
	F("define_directive");
	utf8 *identifier;
	cc__PreToken *t;
	cc__PreToken *idt;

	if (self->env->include->condition != 1) return;

	t = start;
	start = start->next;

	if (start == 0) {
		int32 l = 0;
		while (t->identifier[l]) l++;
		THROW(-100032, "Missing macro identifier")
		CC__WARN("", t->file, t->line, t->char_pos + l);
		CHECK(error);
	}

	while ((start->type & CC_WHITE_SPACE) == CC_WHITE_SPACE) {
		t = start;
		if (start->next == 0) {
			THROW(-100023, "Missing macro identifier")
			CC__WARN("", t->file, t->line, t->char_pos);
			CHECK(error);
		}
		start = start->next;
	}

	idt = t = start;
	if ((start->type & CC_IDENTIFIER) != CC_IDENTIFIER) {
		THROW(-100024, "Macro identifier expected");
		CC__WARN("", t->file, t->line, t->char_pos);
		CHECK(error);
	}

	identifier = start->identifier;
	
	start = start->next;

	if (start != tokens->last && 
		(tokens->last->type & CC_WHITE_SPACE) == CC_WHITE_SPACE)
	{
		tokens->_->delete_end(e__, tokens, tokens->last);
		CHECK(error);
	}

	if (start && start->type == P('(', 0, 0, 0)) {
		cc__PreToken *next = 0;
		cc__PreToken *arg;
		int8 comma = 1;
		int8 stringize = 0;

		arg = start = start->next;

		while (start != 0 &&
			((start->type & CC_WHITE_SPACE) == CC_WHITE_SPACE ||
			(start->type & CC_IDENTIFIER) == CC_IDENTIFIER ||
			start->type == P('.', '.', '.', 0) ||
			start->type == P(',', 0, 0, 0))) 
		{
			t = start;
			next = start->next;	
			if (start->type == P(',', 0, 0, 0))
			{
				if (comma != 0) {
					THROW(-100025, "Parameter identifier"
						" expected");
					CC__WARN("", t->file, t->line,
						t->char_pos); 
					CHECK(error);
				}
				comma = 1;
				if (start == arg) arg = start->next;
				tokens->_->delete_token(e__, tokens, start);
				CHECK(error);
			} else if ((start->type & CC_IDENTIFIER) == 
				CC_IDENTIFIER ||
				start->type == P('.', '.', '.', 0))
			{
				if (comma != 1) {
					THROW(-100026, 
						"',' separator expected");
					CC__WARN("", t->file,
						t->line, t->char_pos);
					CHECK(error);
				}
				comma = 0;
			} else {
				if (start == arg) arg = start->next;
				tokens->_->delete_token(e__, tokens, start);
				CHECK(error);
			}
	
			start = next;
		}

		if (start == 0 || start->type != P(')', 0, 0, 0)) {
			int32 l = 0;
			while (t->identifier[l]) l++;
			THROW(-100027, "')' expected");
			CC__WARN("", t->file, t->line, t->char_pos + l);
			CHECK(error);
		} else {
			next = start->next;
			if (arg == start) arg = 0;
			tokens->_->delete_token(e__, tokens, start);
			CHECK(error);
		}
		start = next;

		// 6.10.3.2 the # token shall be immediately followed by
		// a parameter
		while (next != 0) {
			t = next;
			if (next->type == P('#', 0, 0, 0)) {
				if (stringize != 0) {
					THROW(-100028, "'#' followed by '#'");
					CC__WARN("", t->file, 
						t->line, t->char_pos);
					CHECK(error);
				}
				stringize = 1;
				
			} else if ((next->type & CC_WHITE_SPACE) !=
				CC_WHITE_SPACE)
			{
				if (stringize != 0 &&
					is_arg(e__, self, arg, 
						next->identifier, start) 
					== 0) 
				{
					CHECK(error);
					THROW(-100029, "Expecting parameter "
						"identifier after '#'");
					CC__WARN("", t->file, 
						t->line, t->char_pos);
					CHECK(error);
				}
				stringize = 0;
			}
			next = next->next;
		}
		if (arg != 0) {

		}
		self->macros->_->push_function(e__, self->macros,
				idt, tokens, arg, start);
		CHECK(error);
	} else if ((start->type & CC_WHITE_SPACE) != CC_WHITE_SPACE) {
		int32 l = 0;
		while (t->identifier[l]) l++;
		THROW(-100031, "Missing white space after macro name");
		CC__WARN("", t->file, t->line, t->char_pos + l);
		CHECK(error);
	} else {
		self->macros->_->push_object(e__, self->macros,
				idt, tokens, start);
		CHECK(error);	
	}
	
	return;
error:
	return;
}

static void undef_directive(Ex **e__, cc__PreProcessor *self,
	cc__PreToken *start)
{
	F("undef_directive");
	cc__PreToken *t;

	if (self->env->include->condition != 1) return;

	t = start;	
	start = start->next;

	while (start != 0 && (start->type & CC_WHITE_SPACE) == CC_WHITE_SPACE) {
		t = start;
		if (start->next == 0) {
			THROW(-100033, "Expecting macro identifier");
			CC__WARN("", t->file, t->line, t->char_pos);
			CHECK(error);
		}
		start = start->next;
		t = start;
	}

	if (start == 0 || (start->type & CC_IDENTIFIER) != CC_IDENTIFIER) {
		int32 l = 0;
		while (t->identifier[l]) l++;
		THROW(-100034, "Expecting macro identifier");
		CC__WARN("", t->file, t->line, t->char_pos + l);
		CHECK(error);
	}

	self->macros->_->undef(e__, self->macros, start);
	CHECK(error);
	return;
error:
	return;
}

static void error_directive(Ex **e__, cc__PreProcessor *self,
	cc__PreToken *start)
{
	F("error_directive");
	String *s;
	cc__PreToken *my_start = start;

	if (self->env->include->condition != 1) return;

	s = new_(String);
	CHECK(error1);
	while (start) {
		String x;

		STR(x, start->identifier); 
		s->_->add(e__, s, &x);
		CHECK(error2);
		start = start->next;
	}
	THROW(-100036, "#ERROR: ");
	CC__WARN_DYN(s, my_start->file, my_start->line, my_start->char_pos);
	CHECK(error2);
	delete_(s);
	return;
error2:
	delete_(s);
error1:
	return;
}

static void line_directive(Ex **e__, cc__PreProcessor *self,
	cc__PreTokens *tokens, cc__PreToken *start)
{
	F("line_directive");
	cc__PreToken *tk;
	cc__PreToken *last;
	cc__PreToken *begin = start->previous;

	while (begin->type != P('#', 0, 0, 0) &&
		begin->type != P('%', ':', 0, 0))
	{
		tk = begin->previous;
		tokens->_->delete_token(e__, tokens, begin);
		CHECK(error);
		begin = tk;
	}
	tokens->_->delete_token(e__, tokens, begin);
	CHECK(error);


	if (self->env->include->condition != 1) {
		tokens->_->delete_end(e__, tokens, start);
		CHECK(error);
		return;
	}
	{
		cc__PreToken *l;
		cc__PreToken *t = tokens->next_to_expand_token;
	
		tokens->next_to_expand_token = start->next;
		l = start->previous;
		expand_macros(e__, self, tokens);
		CHECK(error);

		if (t != 0) {
			tokens->next_to_expand_token = t;
		} else {
			tokens->next_to_expand_token = tokens->first;
		}
	}

	last = start;
	tk = start->next;
	tokens->_->delete_token(e__, tokens, last);
	CHECK(error);
	while (tk != tokens->last) {
		last = tk;
		if ((tk->type & CC_WHITE_SPACE) == CC_WHITE_SPACE) {
			tk = tk->next;
			tokens->_->delete_token(e__, tokens, last);
			CHECK(error);
		} else {
			break;
		}
	}
	if (tk == 0 || tk->type != CC_DECIMAL) {
		THROW(-100050, "Expecting line number");
		CC__WARN("", last->file, last->line, last->char_pos);
		CHECK(error);
	}

	/* move the line number token just before "start" */
	tk->type = CC_LINE;
	tk = tk->next;
	while (tk != 0 && tk != tokens->last) {
		last = tk;
		if ((tk->type & CC_WHITE_SPACE) == CC_WHITE_SPACE) {
			tk = tk->next;
			tokens->_->delete_token(e__, tokens, last);
			CHECK(error);
		} else {
			break;
		}
	}
	if (tk == 0 || (tk->type & CC_WHITE_SPACE) == CC_WHITE_SPACE) {
		return;	
	}

	if (tk->type != CC_STRING) {
		THROW(-100051, "Expecting file name");
		CC__WARN("", last->file, last->line, last->char_pos);
		CHECK(error);
	}

	tk->type = CC_FILE;
	if (tk->next != 0) {
		tokens->_->delete_end(e__, tokens, tk->next);
		CHECK(error);
	}
	return;
error:
	return;
}

static int8 compare_utf8(utf8 *a, utf8 *b)
{
	while (*a != 0  && *a == *b) {
		a++;
		b++;
	}
	return (int8)(*a - *b);
}

static void compare_tokens(Ex **e__, cc__PreProcessor *self,
	cc__PreToken *reference, cc__PreToken *start, cc__PreToken *end)
{
	F("compare_tokens");

	while (start != 0 && start != end && 
		((start->type & CC_WHITE_SPACE) == CC_WHITE_SPACE)) 
	{
		start = start->next;
	}
	while (reference != 0 && 
		(reference->type & CC_WHITE_SPACE) == CC_WHITE_SPACE) 
	{
		reference = reference->next;
	}

	while (reference != 0 && start != 0 && start != end) {
		int8 is_not_eq;
		
		is_not_eq  = compare_utf8(reference->identifier, 
				start->identifier);

		if (is_not_eq != 0
			&& 
			((reference->type & CC_WHITE_SPACE) != CC_WHITE_SPACE
			|| (start->type & CC_WHITE_SPACE) != CC_WHITE_SPACE))
		{
			String *s;
			String m;

			STR(m, "'%s' != '%s'");
			s = new_(String);
			CHECK(error1);			
			s->_->printf(e__, s, &m, reference->identifier, 
				start->identifier);
			CHECK(error2);
			THROW(10, "c5_compare_tokens: tokens not equal");
			CC__WARN_DYN(s, reference->file, reference->line,
				reference->char_pos); 
			CC__WARN("", start->file, start->line, start->char_pos);
error2:
			delete_(s);
error1:
			CHECK(error);
		}
		start = start->next;
		while (start != 0 && start != end && 
			(start->type & CC_WHITE_SPACE) == CC_WHITE_SPACE) 
		{
			start = start->next;
		}
		reference = reference->next;
		while (reference != 0 && 
			(reference->type & CC_WHITE_SPACE) == CC_WHITE_SPACE) 
		{
			reference = reference->next;
		}
	}

	if (reference != 0) {
		THROW(10, "c5_compare_tokens: not enough tokens");
		CC__WARN("", reference->file, reference->line, 
			reference->char_pos);
		CHECK(error);
	} else if (start != end) {
		THROW(10, "c5_compare_tokens: too many tokens");
		CC__WARN("", start->file, start->line, start->char_pos);
		CHECK(error);
	}
	return;
error:
	return;
}

				
static int8 pragma_exec(Ex **e__, cc__PreProcessor *self,
	cc__PreTokens *tokens, cc__PreToken *start)
{
	F("pragma_exec");
	utf8 *ptr;

	ptr = start->identifier;

	while (IS_WHITE_SPACE(*ptr)) ptr++;

	switch (ptr[0]) {
	case 'S':
		if (compare_utf8(ptr, (utf8*)"STDC") == 0) {
			ptr += 4;
			while (IS_WHITE_SPACE(*ptr)) ptr++;
			if (compare_utf8(ptr, (utf8*)"FP_CONTRACT") == 0) {
				/* 7.12.2 */
				ptr += 11;
				while (IS_WHITE_SPACE(*ptr)) ptr++;
				if (compare_utf8(ptr, (utf8*)"ON") == 0) {
					self->env->pragma_stdc_fp_contract = 1;
				} else if (compare_utf8(ptr, (utf8*)"OFF")
					== 0) 
				{
					self->env->pragma_stdc_fp_contract = 0;
				} else if (compare_utf8(ptr, (utf8*)"DEFAULT")
					== 0) 
				{
					self->env->pragma_stdc_fp_contract = 0;
				}
				tokens->_->delete_token(e__, tokens, start);
				CHECK(error);
			} else if (compare_utf8(ptr, (utf8*)"FENV_ACCESS")
				== 0) 
			{
				/* 7.6.1 */
				ptr += 11;
				while (IS_WHITE_SPACE(*ptr)) ptr++;
				if (compare_utf8(ptr, (utf8*)"ON") == 0) {
					self->env->pragma_stdc_fenv_access = 1;
				} else if (compare_utf8(ptr, (utf8*)"OFF")
					== 0) 
				{
					self->env->pragma_stdc_fenv_access = 0;
				} else if (compare_utf8(ptr, (utf8*)"DEFAULT")
					== 0) 
				{
					self->env->pragma_stdc_fenv_access = 0;
				}
				tokens->_->delete_token(e__, tokens, start);
				CHECK(error);
			} else if (compare_utf8(ptr, (utf8*)"CX_LIMITED_RANGE")
				== 0) 
			{
				/* 7.3.4 */
				ptr += 16;
				while (IS_WHITE_SPACE(*ptr)) ptr++;
				if (compare_utf8(ptr, (utf8*)"ON") == 0) {
					self->env->pragma_stdc_cx_limited_range
						 = 1;
				} else if (compare_utf8(ptr, (utf8*)"OFF")
					== 0) 
				{
					self->env->pragma_stdc_cx_limited_range
						 = 0;
				} else if (compare_utf8(ptr, (utf8*)"DEFAULT")
					 == 0) 
				{
					self->env->pragma_stdc_cx_limited_range
						 = 0;
				}
				tokens->_->delete_token(e__, tokens, start);
				CHECK(error);
			}
		}
		break;
	case 'c':
		break;
	case 'o':
		if (compare_utf8(ptr, (utf8*)"once") == 0) {
			tokens->_->delete_token(e__, tokens, start);
			CHECK(error);
			self->env->include->_->include_once(e__,
				self->env->include);
			CHECK(error);
		}
		break;
	}
	return 0;
error:
	return 0;

}

static void add_escaped(Ex **e__, String *s, utf8 *id)
{
	F("add_escaped");
	utf8 *ptr1 = id;
	utf8 *ptr2 = id;
	String sl;
	String st;

	STR(sl, "\\");
	STR(st, "");

	while (*ptr1 != 0) {
		while (*ptr2 != 0) {
			st.cstr = ptr1;
			if (*ptr2 == '"') {
				*ptr2 = 0;
				s->_->add(e__, s, &st);
				*ptr2 = '"';
				s->_->add(e__, s, &sl);
				break;
			} else if (*ptr2 == '\'') {
				*ptr2 = 0;
				s->_->add(e__, s, &st);
				*ptr2 = '\'';
				s->_->add(e__, s, &sl);
				break;
			} else if (*ptr2 == '\n') {
				String s1;

				STR(s1, "\\n");
				*ptr2 = 0;
				s->_->add(e__, s, &st);
				ptr2++;
				s->_->add(e__, s, &s1);
				break;
			} else if (*ptr2 == '\r') {
				String s1;

				STR(s1, "\\r");
				*ptr2 = 0;
				s->_->add(e__, s, &st);
				ptr2++;
				s->_->add(e__, s, &s1);
				break;
			}					
			ptr2++;
		}
		if (*ptr2 == 0) {
			s->_->add(e__, s, &st);
		}
		ptr1 = ptr2; 
	}
	CHECK(error);
	return;
error:
	return;
	
}

static int8 pragma_directive(Ex **e__, cc__PreProcessor *self,
	cc__PreTokens *tokens, cc__PreToken *start, cc__PreToken *first)
{
	F("pragma_directive");
	cc__PreToken *tk;
	String *s;

	if (self->env->include->condition != 1) return 0;

	tk = first->next;
	while (tk != start) {
		cc__PreToken *n = tk->next;
	
		tokens->_->delete_token(e__, tokens, tk);
		CHECK(error1);
		tk = n;
		
	}
	tk = start->next;
	tokens->_->delete_token(e__, tokens, start);
	CHECK(error1);
	
	while (tk != 0 && (tk->type & CC_WHITE_SPACE) == CC_WHITE_SPACE) {
		cc__PreToken *n = tk->next;
	
		tokens->_->delete_token(e__, tokens, tk);
		CHECK(error1);
		tk = n;
	} 
	if (tk != 0) {
		utf8 *ptr = tk->identifier;

		if (compare_utf8(ptr, (utf8*)"c5_clear_tokens") == 0) {
			tokens->_->delete_end(e__, tokens, tokens->first);
			CHECK(error1);
			return 1; /* don't delete tokens again */
		} else if (compare_utf8(ptr, (utf8*)"c5_compare_tokens") == 0) {
			cc__PreToken *n;

			n = tk->next;
			if (n != 0) n = n->next;
			compare_tokens(e__, self, n, tokens->first, first);
			CHECK(error1);
			return 0;			
		}
	}

	s = new_(String);
	CHECK(error1);

	while (tk != 0) {
		cc__PreToken *n = tk->next;

		if (!((tk->type & CC_WHITE_SPACE) == CC_WHITE_SPACE &&
			tk->next == 0))
		{
			add_escaped(e__, s, tk->identifier);
			CHECK(error2);
		}
		if (tk != first) {
			tokens->_->delete_token(e__, tokens, tk);
			CHECK(error2);
		}
		tk = n;
	}
	Gc__free(e__, first->identifier);
	CHECK(error2);
	first->identifier = s->cstr;
	first->type = CC_PRAGMA;
	s->cstr = 0;
	delete_(s);
	CHECK(error1);
	pragma_exec(e__, self, tokens, first);
	CHECK(error1);
	return 1;
error2:
	delete_(s);
error1:
	return 0;
}

/**
 * 5.1.1.2 Translation phases 4)
 * execute preprocessing directives.
 * get the first token after a "#" and then call the corresponding 
 * directive method.
 */
static void execute_directives(Ex **e__, cc__PreProcessor *self,
	cc__PreTokens *tokens)
{
	F("execute_directive");
	utf8 *cstr = 0;
	utf8 c;
	int8 ok = 0;
	cc__PreToken *tk;
	cc__PreToken *first = 0;
	int8 non_directive = 1;
	cc__PreToken *prev;

	tk = tokens->last;

	while (tk != 0 && ok == 0) {
		while (tk != 0 && tk->type != P('#', 0, 0, 0) &&
			tk->type != P('%', ':', 0, 0))
		{
			tk = tk->previous;
		}
		first = tk;
		if (tk != 0) tk = tk->previous;
		while (tk != 0 && ok == 0) {
			if ((tk->type & CC_WHITE_SPACE) == CC_WHITE_SPACE) {
				if (tk->type == CC_WHITE_SPACE) {
					utf8 *p = tk->identifier;
					while (*p != 0 && *p != '\r' &&
						*p != '\n')
					{
						p++;
					}
					if (*p != 0) {
						ok = 1;
						break;
					}
				} else if (tk->type == CC_LINE_COMMENT) {
					ok = 1;
					break;
				}
			} else {
				break;
			}
			tk = tk->previous;
		}
		if (tk == 0) ok = 1;
	}

	if (ok == 0 || first == 0) {
		THROW(10, "C compiler fatal error.");
		CHECK(error);
	}
	prev = first->previous;	
	tk = first->next;
	while (tk != 0) {
		if ((tk->type & CC_WHITE_SPACE) == CC_WHITE_SPACE) {
			tk = tk->next;
		} else {
			break;
		}
	}


	if (tk == 0) {
		non_directive = 0; // 6.10 # new-line
		c = 0;
	} else {
		cstr = tk->identifier;
		c = cstr[0];
	}
	if (c == 'i') {
		c = cstr[1];
		if (c == 'n') {
			if (cstr[2] == 'c' && cstr[3] == 'l' && 
				cstr[4] == 'u' && cstr[5] == 'd' &&
				cstr[6] == 'e' && cstr[7] == 0)
			{
				// "include"
				include_directive(e__, self, tokens, tk);
				CHECK(error);
				return;
			}	
		} else if (c == 'f') {
			if (cstr[2] == 0) {
				// "if"
				if_directive(e__, self, tk);
				CHECK(error);
				non_directive = 0;
			} else if (cstr[2] == 'd' && cstr[3] == 'e' &&
				cstr[4] == 'f' && cstr[5] == 0)
			{
				// "ifdef"
				ifdef_directive(e__, self, tk);
				CHECK(error);
				non_directive = 0;
			} else if (cstr[2] == 'n' && cstr[3] == 'd' &&
				cstr[4] == 'e' && cstr[5] == 'f' &&
				cstr[6] == 0)
			{
				// "ifndef"
				ifndef_directive(e__, self, tk);
				CHECK(error);
				non_directive = 0;
			}
		}
	} else if (c == 'd') {
		if (cstr[1] == 'e') {
			if (cstr[2] == 'f' && cstr[3] == 'i' && 
				cstr[4] == 'n' && cstr[5] == 'e' &&
				cstr[6] == 0)
			{
				// "define"
				define_directive(e__, self, tokens, tk);
				CHECK(error);
				non_directive = 0;
			}	
		}
	} else if (c == 'e') {
		if (cstr[1] == 'l') {
			if (cstr[2] == 's' && cstr[3] == 'e' && 
				cstr[4] == 0)
			{
				// "else"
				else_directive(e__, self, tk);
				CHECK(error);
				non_directive = 0;
			} else if (cstr[2] == 'i' && cstr[3] == 'f' && 
				cstr[4] == 0)
			{
				// "elif"
				elif_directive(e__, self, tk);
				CHECK(error);
				non_directive = 0;
			}
		} else if (cstr[1] == 'n') {
			if (cstr[2] == 'd' && cstr[3] == 'i' && 
				cstr[4] == 'f' && cstr[5] == 0)
			{
				// "endif"
				endif_directive(e__, self, tk);
				CHECK(error);
				non_directive = 0;
			}
		} else if (cstr[1] == 'r' && cstr[2] == 'r' &&
			cstr[3] == 'o' && cstr[4] == 'r' &&
			cstr[5] == 0)
		{  	
			error_directive(e__, self, tk);
			CHECK(error);
			non_directive = 0;
		}
	} else if (c == 'p') {
		if (cstr[1] == 'r') {
			if (cstr[2] == 'a' && cstr[3] == 'g' && 
				cstr[4] == 'm' && cstr[5] == 'a' &&
				cstr[6] == 0)
			{
				// "pragma"
				non_directive = pragma_directive(e__, self, 
					tokens, tk, first);
				CHECK(error);
			}	
		}
	} else if (c == 'u') {
		if (cstr[1] == 'n') {
			if (cstr[2] == 'd' && cstr[3] == 'e' && 
				cstr[4] == 'f' && cstr[5] == 0)
			{
				// "undef"
				undef_directive(e__, self, tk);
				CHECK(error);
				non_directive = 0;
			}	
		}

	} else if (c == 'l') {
		if (cstr[1] == 'i') {
			if (cstr[2] == 'n' && cstr[3] == 'e' && cstr[4] == 0) {
				// "line"
				line_directive(e__, self, tokens, tk);
				CHECK(error);
				tokens->next_to_expand_token = tokens->last;
				non_directive = 1;
			}	
		}
	}


	if (non_directive != 0 && self->env->include->condition == 1) {
		// 6.10 this is an non-directive so we keep all the tokens!
		return;
	}
	if (prev) {
		first = prev->next;
	} else {
		first = tokens->first;
	}
	// remove the directive tokens!
	tokens->_->delete_end(e__, tokens, first);
	CHECK(error);
error:
	return;
}


static void pragma_found(Ex **e__, cc__PreProcessor *self,
	cc__PreTokens *tokens, cc__PreToken *start)
{
	F("pragma_found");
	cc__PreToken *t;
	int32 par = 1;
	String *s;
	String st;	

	STR(st, "");
	t = start->next;

	while (t != 0 && (t->type & CC_WHITE_SPACE) == CC_WHITE_SPACE) {
		t = t->next;
	}
	if (t == 0 || t->type != P('(', 0, 0, 0)) {
		THROW(-100052, "Expecting '(' after _Pragma keyword.");
		CC__WARN("", start->file, start->line, start->char_pos + 7);
		CHECK(error1);
	}
	s = new_(String);
	CHECK(error1);

	t = t->next;
	while (t != 0 && par != 0) {
		if (t->type == P('(', 0, 0, 0)) {
			par++;
		} else if (t->type == P(')', 0, 0, 0)) {
			par--;
		} else if ((t->type & CC_WHITE_SPACE) == CC_WHITE_SPACE) {
		} else if (t->type == CC_STRING) {
			st.cstr = t->identifier;
			s->_->add(e__, s, &st);
			CHECK(error2);
		} else {
			THROW(-100053, "Expecting string here.");
			CC__WARN("", t->file, t->line, t->char_pos);
			CHECK(error1);
		}
		t = t->next;
	}
	if (par != 0) {
		delete_(s);
		CHECK(error1);
		return;
	}
	Gc__free(e__, start->identifier);
	CHECK(error2);
	start->identifier = s->cstr;
	start->type = CC_PRAGMA;
	s->cstr = 0;
	delete_(s);
	CHECK(error1);
	tokens->next_to_execute_pragma = start;
	t = t->previous;
	while (t != start) {
		cc__PreToken *p = t->previous;

		tokens->_->delete_token(e__, tokens, t);
		CHECK(error1);
		t = p;
	}
	return;
error2:
	delete_(s);
error1:
	return;
}

static void execute_pragma(Ex **e__, cc__PreProcessor *self,
	cc__PreTokens *tokens)
{
	F("execute_pragma");
	cc__PreToken *t;
	
	t = tokens->next_to_execute_pragma;
	if (t == 0) {
		t = tokens->first;
	}

	while (t != 0) {
		if (t == self->tokens->next_to_expand_token) {
			self->tokens->next_to_execute_pragma = t;
			return;
		}
		if (t->type == CC_EXPANDED_ID) {
			utf8 *ptr = t->identifier;
			utf8 *p = (utf8*)"_Pragma";
			int32 i = 0;
		
			while (p[i] != 0 && p[i] == ptr[i]) i++;
			if (p[i] == ptr[i]) {
				pragma_found(e__, self, tokens, t);
				CHECK(error);
				if (tokens->next_to_execute_pragma != t) {
					return;
				}
				pragma_exec(e__, self, tokens, t);
				CHECK(error);
			}
		}
		t = t->next;
	}
	tokens->next_to_execute_pragma = tokens->last;
	return;
error:
	return;
}

/**
 * 5.1.1.2.3 decompose source in preprocessing tokens
 */
extern void cc__PreProcessor__process(Ex **e__, cc__PreProcessor *self)
{
	F("cc__PreProcessor__process");
	utf8 *cstr = self->cstr;
	int32 size = self->size;
	int32 i = 0;


	// second and next lines of a multi-line comment
	if (self->in_comment != 0) {
		while (i < size - 1 && 
			!(cstr[i] == '*' && cstr[i + 1] == '/'))
		{
			i++;
		}
		if (i < size - 1 && 
			(cstr[i] == '*' && cstr[i + 1] == '/')) 
		{
			push_comment(e__, self, cstr, i);
			CHECK(error);
			i += 2;
			self->in_comment = 0;
		} else {
			return;
		}
	}

	tokenize_cstr(e__, self, cstr + i, size - i);
	CHECK(error);

/*
	dump(e__, self);
if (self->has_sharp != 0) {
ECHO("SAPE ");
}
	if (self->env->include->condition == 1) {
	ECHO("ok\r\n");
	} else {
	ECHO("bad\r\n");

	}
*/	
	if (self->in_comment == 0) {

		if (self->has_sharp != 0) {
			execute_directives(e__, self, self->tokens);
			CHECK(error);
		}
			{
			expand_macros(e__, self, self->tokens);
//			self->tokens->next_to_expand_token = self->tokens->last;
			CHECK(error);
			execute_pragma(e__, self, self->tokens);
			CHECK(error);
			self->env->_->parse(e__, self->env);
			CHECK(error);
		}
		self->has_sharp = 0;
	}
error:
	return;
}

static void set_env(Ex **e__, cc__PreProcessor *self, cc__Env *env)
{
	F("set_env");

	self->env = env;
	self->macros->_->set_env(e__, self->macros, env);
	CHECK(error);

error:
	return;
}

static void run(Ex **e__, cc__PreProcessor *self)
{
	F("run");
	int8 r;
	
	CHECK(error);
	do {
		r = process_loop(e__, self);
		CHECK(error);
	} while (r == 0);
	return;
error:
	return;
}

static void io__i_serializable(Ex **e__, cc__PreProcessor *self, io__ISerializable *out)
{
	(void)e__;
	out->_ = &io__ISerializable__;
	out->__self__ = (Object*)self;
}

static void serialize(Ex **e__, io__ISerializable *itf, io__Serializer *out)
{
	Buffer__.serialize(e__, itf, out);
}

static void unserialize(Ex **e__, io__ISerializable *itf, io__Serializer *in)
{
	Buffer__.unserialize(e__, itf, in);
}

static void i_comparable(Ex **e__, cc__PreProcessor *self, IComparable *out)
{
	(void)e__;
	out->_ = &IComparable__;
	out->__self__ = (Object*)self;	
}

static int8 compare_to(Ex **e__, IComparable *itf, Object *in)
{
	return Buffer__.compare_to(e__, itf, in);
}

INIT_EXPORT cc__PreProcessor__class cc__PreProcessor__  = {
	// Object
	(utf8*)"cc__PreProcessor",
	sizeof(cc__PreProcessor),
	0,
	__init__,
	__ctor__,
	__dtor__,
	hash_code,
	to_string,

	// String
	io__i_serializable,
	serialize,
	unserialize,
	i_comparable,
	compare_to,
	add,
	0, // set,
	0, // clear,
	0, // get_cstr,
	0, // get_size,
	0, // _printf,
	0, // index_of,

	// Buffer
	0, // add_data,

	// PreProcessor
	process_file,
	set_env,
	get_type_of_cstr,
	run,
	process_loop,
	cc__PreProcessor__process
};

static io__ISerializable__class io__ISerializable__  = {
	// Object
	0, 0, 0, 0, 0, 0, 0, 0,

	// io__ISerializable
	serialize,
	unserialize
};

static IComparable__class IComparable__  = {
	// Object
	0, 0, 0, 0, 0, 0, 0, 0,

	// IComparable
	compare_to
};