/* "$Id: $"
 *
 *                          24 July MMVIII PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <cc/PreTokens.h>
#include <cc/PreProcessor.h>

static void delete_token(Ex **e__, cc__PreTokens *self, cc__PreToken *token);

static void __init__(Ex **e__)
{
	(void)e__;
	cc__PreTokens__.super = &Object__;
	return;
}

static void __ctor__(Ex **e__, cc__PreTokens *self)
{
	(void)e__;
	self->first = 0;
	self->last = 0;	
	self->next_to_expand_token = 0;
	self->next_to_execute_pragma = 0;
	return;
}

static void __dtor__(Ex **e__, cc__PreTokens *self)
{
	F("__dtor__");
	cc__PreToken *first = self->first;

	while (first != 0) {
		cc__PreToken *next = first->next;

		delete_(first);
		CHECK(error1);
error1:
		first = next;
	}

	return;
}

static uint32 hash_code(Ex **e__, cc__PreTokens *self)
{
	(void)e__;
#ifdef _MSC_VER
	return *((uint32*)&self) >> 4;
#else 
	return (uint32)self >> 4;
#endif
} 

static void to_string(Ex **e__, cc__PreTokens *self, String *out)
{
	F("to_string");
	String s;

	STR(s, self->_->__name__);
	out->_->add(e__, out, &s);
	CHECK(error);
error:
	return;
}

/*
static void check(Ex **e__, cc__PreTokens *self)
{
	F("check");
	cc__PreToken *tk;
	cc__PreToken *p = 0;
	cc__PreToken *n = 0;

	if (self->first && self->first->previous != 0) {
		THROW(15, "C compiler internal error.");
		CHECK(error);
	}
	if (self->last && self->last->next != 0) {
		THROW(15, "C compiler internal error.");
		CHECK(error);
	}

	if (self->first != 0 && self->last == 0) {
		THROW(15, "C compiler internal error.");
		CHECK(error);
	}
	if (self->first == 0 && self->last != 0) {
		THROW(15, "C compiler internal error.");
		CHECK(error);
	}

	tk = self->first;
	while (tk != 0) {
		if (tk->previous != p) {
			THROW(15, "C compiler internal error.");
			CHECK(error);
		}
		if (tk->next != 0 && tk->next->previous != tk) {
			THROW(15, "C compiler internal error.");
			CHECK(error);
		}
		p = tk;
		tk = tk->next;
	}
	if (p != self->last) {
		THROW(15, "C compiler internal error.");
		CHECK(error);

	}
	tk = self->last;
	while (tk != 0) {
		if (tk->next != n) {
			THROW(15, "C compiler internal error.");
			CHECK(error);
		}
		if (tk->previous != 0 && tk->previous->next != tk) {
			THROW(15, "C compiler internal error.");
			CHECK(error);
		}
		n = tk;
		tk = tk->previous;
	}
	if (n != self->first) {
		THROW(15, "C compiler internal error.");
		CHECK(error);

	}

	return;
error:
	return;
}
*/

EXPORT void cc__PreTokens__push(Ex **e__, cc__PreTokens *self, utf8 *name, 
	int32 size, uint32 type, uint32 file, uint32 line, uint32 char_pos)
{
	F("push");
	cc__PreToken *tk;

	if (type == CC_END_TOKEN) return;

	tk = new_(cc__PreToken);
	CHECK(error1);

	tk->identifier = (utf8*)Gc__malloc(e__, size + 1);
	CHECK(error2);

	tk->identifier[size] = 0;
	while (size > 0) {
		size--;
		tk->identifier[size] = name[size];
	}
	tk->type = type;
	tk->file = file;
	tk->line = line;
	tk->char_pos = char_pos;
	tk->replace_index = -1;

	
	if (self->last) {
		cc__PreToken *l;
		l = tk->previous = self->last;
		tk->next = 0;
		self->last->next = tk;
		self->last = tk;
		if (l->type == CC_END_TOKEN) {
			delete_token(e__, self, l);
			CHECK(error1);
		}
	} else {
		tk->previous = 0;
		tk->next = 0;
		self->last = tk;
		self->first = tk;
		self->next_to_execute_pragma = tk;
		self->next_to_expand_token = tk;
	}
	//check(e__, self);
	//CHECK(error1);
	return;
error2:
	delete_( tk);	
error1:
	return;
}


/**
 * The "by" tokens must have been alocated because they will be freed here!
 */
static void replace(Ex **e__, cc__PreTokens *self,
		cc__PreToken *from, cc__PreToken *to, cc__PreTokens *by)
{
	F("replace");
	cc__PreToken *c = 0;
	cc__PreToken *prev;
	uint32 file = from->file;
	uint32 line = from->line;

	prev = from->previous;
	while (from != to && from != c) {
		c = from;
		delete_token(e__, self, from);
		CHECK(error);
		if (prev) {
			from = prev->next;
		} else {
			from = self->first;
		}	
	}
	delete_token(e__, self, to);
	CHECK(error);

	if (by->first) {
		c = by->first;
		while (c != 0) {
			c->line = line;
			c->file = file;
			c = c->next;
		}

		if (prev != 0) {
			by->last->next = prev->next;
			by->first->previous = prev;
			if (prev->next != 0) {
				prev->next->previous = by->last;
			} else {
				self->last = by->last;
			}
			prev->next = by->first;
		} else {
			if (self->first != 0) {
				by->last->next = self->first;
				self->first->previous = by->last;
			} else {
				self->last = by->last;
				by->last->next = 0;
			}
			self->first = by->first;
			by->first->previous = 0;
		}
	}
	by->first = 0;
	by->last = 0;
	delete_(by);
	CHECK(error);
	//check(e__, self);
	//CHECK(error);

error:
	return;
}

static void copy(Ex **e__, cc__PreTokens *self, cc__PreTokens **out)
{
	F("copy");
	cc__PreTokens *pt;
	cc__PreToken *c;

	pt = new_(cc__PreTokens);
	CHECK(error1);
	c = self->first;
	while (c != 0) {
		utf8 *n;

		n = c->identifier;
		while (*n != 0) n++;
		cc__PreTokens__push(e__, pt, c->identifier,
			(int32)(n - c->identifier), 
			c->type, c->file, c->line, c->char_pos);
		CHECK(error2);
		c = c->next;
	}
	*out = pt;
	return;
error2:
	delete_(pt);
error1:
	return;
}

static void delete_end(Ex **e__, cc__PreTokens *self, cc__PreToken *from)
{
	F("delete_end");

	cc__PreToken *p = 0;

	if (from != 0) p = from->previous;

	while (from != 0) {
		delete_token(e__, self, from);
		CHECK(error);
		if (p != 0) {
			from = p->next;
		} else {
			from = self->first;
			if (from != 0 && from == self->last && 
				from->type == CC_END_TOKEN)
			{
				break;
			}
		}
	}
	//check(e__, self);
	//CHECK(error);

error:
	return;
}

static void delete_token(Ex **e__, cc__PreTokens *self, cc__PreToken *token)
{
	F("delete_token");
	cc__PreToken *p;
	cc__PreToken *n;

	if (token == 0) return;


	p = self->first;
	while (p != 0 && p != token) p = p->next;
	if (p != token) {
		THROW(15, "C compiler internal error.");
		CHECK(error);
	} 
	//check(e__, self);
	//CHECK(error);

	if (token == self->next_to_execute_pragma) {
		if (token->next != 0) {
			self->next_to_execute_pragma = token->next;
		} else {
			/* dont delete the token because we are at the end
			 * of the token chain. 
			 */
			token->identifier[0] = 0;
			token->type = CC_END_TOKEN;
			return;
		}
	}
	if (token == self->next_to_expand_token) {
		if (token->next != 0) {
			self->next_to_expand_token = token->next;
		} else {
			token->identifier[0] = 0;
			token->type = CC_END_TOKEN;
			return;
		}
	}

	p = token->previous;
	n = token->next;

	if (p != 0) {
		p->next = n;
	} else {
		// token is self->first
		self->first = n;
	}
	if (n != 0) {
		n->previous = p;
	} else {
		// token is self->last
		self->last = p;
	}

	if (self->first != 0 && self->last == 0) {
		self->last = self->first; // FIXME
	} else if (self->first == 0 && self->last != 0) {
		self->first = self->last;
	}
	delete_(token);
	CHECK(error);
/*	check(e__, self);
	if (*e__ != 0) {
		ECHO("");
	}
	CHECK(error);
*/

error:
	return;
}

static void get_pos(Ex **e__, cc__PreTokens *self, cc__PreToken *token, 
		uint32 *file, uint32 *line, uint32 *char_pos)
{
	(void)e__;
	(void)self;

	*file = token->file;
	*line = token->line;
	*char_pos = token->char_pos;
	return;
}

static void extract(Ex **e__, cc__PreTokens *self, cc__PreToken *from,
	cc__PreToken *to, cc__PreTokens **out)
{
	F("extract");
	cc__PreTokens *pt;
	cc__PreToken *c;
	cc__PreToken *prev;

	pt = new_(cc__PreTokens);
	CHECK(error1);

	prev = from->previous;

	if (from == self->first) {
		self->first = to->next;
		if (self->first != 0) {
			self->first->previous = 0;
		}
	}
	if (to == self->last) {
		self->last = from->previous;
		if (self->last != 0) {
			self->last->next = 0;
		}
	}
	if (from->previous != 0) {
		from->previous->next = to->next;
	}
	if (to->next != 0) {
		to->next->previous = from->previous;
	}
	to->next = 0;
	from->previous = 0;
	pt->first = from;
	pt->last = to;
	c = from;
	while (c != 0) {
		if (c == self->next_to_execute_pragma) {
			if (prev != 0) {
				if (prev->next != 0) {
					self->next_to_execute_pragma = 
						prev->next;
				} else {
					cc__PreTokens__push(e__, pt, 
						(utf8*)"", 0,
						c->type, c->file,
						c->line, c->char_pos);
					CHECK(error2);
					self->next_to_execute_pragma = 
						self->last;
				}
			} else {
				self->next_to_execute_pragma = self->first;
			}
		}
		if (c == self->next_to_expand_token) {
			if (prev != 0) {
				if (prev->next != 0) {
					self->next_to_expand_token = 
						prev->next;
				} else {
					cc__PreTokens__push(e__, pt,
						(utf8*)"", 0,
						c->type, c->file,
						c->line, c->char_pos);
					CHECK(error2);
					self->next_to_expand_token = 
						self->last;
				}
			} else {
				self->next_to_expand_token = self->first;
			}
		}
		c = c->next;
	}
	*out = pt;
	return;
error2:
	delete_(pt);
error1:
	return;

}

static void get_first_token(Ex **e__, cc__PreTokens *self, cc__PreToken **out)
{
	F("get_first_token");
	cc__PreToken *c;

	(void)e__;
	(void)__func__;

	c = self->first;
	if (c == self->next_to_execute_pragma || 
		c == self->next_to_expand_token)
	{
		*out = 0;
		return;
	}
	*out = c;
	self->first = c->next;
	if (c == self->last) {
		self->last = 0;
	}
	c->previous = 0;
	c->next = 0; 
}

INIT_EXPORT cc__PreTokens__class cc__PreTokens__  = {
	// Object
	(utf8*)"cc__PreTokens",
	sizeof(cc__PreTokens),
	0,
	__init__,
	__ctor__,
	__dtor__,
	hash_code,
	to_string,

	// PreTokens
	cc__PreTokens__push,
	replace,
	copy,
	delete_end,
	delete_token,
	get_pos,
	extract,
	get_first_token
};

