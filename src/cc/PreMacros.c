/* "$Id: $"
 *
 *                          27 July MMVIII PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <cc/PreMacros.h>
#include <cc/PreProcessor.h>

#define IS_WHITE_SPACE(c) ( \
	c == ' ' || \
	c == '\t' || \
	c == '\r' || \
	c == '\n' || \
	c == '\v' \
)

#define P(a, b, c, d) ((a & 0xff)  | ((b << 8) & 0xff00) \
	| ((c << 16) & 0xff0000) | ((d << 24) & 0xff000000))

static void expand_macros_or_args(Ex **e__, cc__PreMacros *self, 
	cc__PreTokens *tokens, int8 is_arguments);
static cc__PreTokens *insert_special(Ex **e__, cc__PreMacros *self,
	cc__PreToken *identifier);

static void __init__(Ex **e__)
{
	(void)e__;

	cc__PreMacros__.super = &Object__;
	return;
}


static void __ctor__(Ex **e__, cc__PreMacros *self)
{
	F("__ctor__");
	cc__PreToken f = {0, (utf8*)"__FILE__", CC_IDENTIFIER, 0};
	cc__PreToken l = {0, (utf8*)"__LINE__", CC_IDENTIFIER, 0};

	self->hash = new_(Hash);
	CHECK(error1);

	self->env = 0;
	self->replaced = new_(cc__PreMacroReplaced);
	CHECK(error2);

	self->file_def = insert_special(e__, self, &f);
	CHECK(error3);
	self->line_def = insert_special(e__, self, &l);
	CHECK(error4);
	return;
error4:
	delete_(self->file_def);
error3:
	delete_(self->replaced);
error2:
	delete_(self->hash);
error1:
	return;
}

static void __dtor__(Ex **e__, cc__PreMacros *self)
{
	F("__dtor__");

	self->hash->_->clear_objects(e__, self->hash);
	CHECK(error1);
error1:
	delete_(self->hash);
	CHECK(error2);
error2:
	delete_(self->replaced);
	CHECK(error3);
error3:
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

static uint32 hash_code(Ex **e__, cc__PreMacros *self)
{
	(void)e__;
#ifdef _MSC_VER
	return *((uint32*)&self) >> 4;
#else 
	return (uint32)self >> 4;
#endif
} 

static void to_string(Ex **e__, cc__PreMacros *self, String *out)
{
	F("to_string");
	String s;

	STR(s, self->_->__name__);
	out->_->add(e__, out, &s);
	CHECK(error);
error:
	return;
}

static void insert_function(Ex **e__, cc__PreMacros *self,
	cc__PreToken *identifier, utf8 **args,
	int32 nb_args, cc__PreTokens *tk, int8 is_object)
{
	F("insert_function");
	cc__PreMacro *m;
	String s;


	m = new_(cc__PreMacro);
	CHECK(error1);
	m->_->set(e__, m, tk, args, nb_args, is_object, identifier->file,
		identifier->line, identifier->char_pos);
	CHECK(error2);
 	
	STR(s, identifier->identifier);
	self->hash->_->set(e__, self->hash, &s, m, (void**)&m);
	CHECK(error2);
	if (m != 0) {
		delete_(m);
		CHECK(error1);
		THROW(15, "C compiler internal error.");
		CC__WARN("", identifier->file, identifier->line,
			identifier->char_pos);
		CHECK(error1);
	}
	return;
error2:
	delete_(m);
error1:
	return;
}

static cc__PreTokens *insert_special(Ex **e__, cc__PreMacros *self,
	cc__PreToken *identifier)
{
	F("insert_special");
	cc__PreTokens *tk;

	tk = new_(cc__PreTokens);
	CHECK(error1);

	tk->_->push(e__, tk, (utf8*)"", 0, CC_STRING, 0, 0, 0);
	CHECK(error2);
	insert_function(e__, self, identifier, 0, 0, tk, 1);
	CHECK(error2);	
	return tk;
error2:
	delete_(tk);
error1:
	return 0;
}

static void check_equal_function(Ex **e__, cc__PreMacros *self,
	cc__PreMacro *old, utf8 **args, int32 nb_args, cc__PreTokens *tk,
	int8 is_object, cc__PreToken *idt)

{
	F("check_equal_function");
	cc__PreToken *o;
	cc__PreToken *n;
	int32 i;

	if (old->is_object != is_object) {
		THROW(-100030, "Macro with different type already defined");
		CC__WARN("", idt->file, idt->line, idt->char_pos);
		CC__WARN("Previously defined here: ", 
			old->file, old->line, old->char_pos);
		CHECK(error);
	}
	if (old->nb_parameters != nb_args) {
		THROW(-100045, "Macro with different parameter"
				" count already defined");
		CC__WARN("", idt->file, idt->line, idt->char_pos);
		CC__WARN("Previously defined here: ", 
			old->file, old->line, old->char_pos);
		CHECK(error);
	}
	for (i = 0; i < nb_args; i++) {
		int32 j = 0;
		while (args[i][j] != 0 && 
			args[i][j] == old->parameters[i][j])
		{
			j++;
		}
		if (args[i][j] != old->parameters[i][j]) {
			THROW(-100046, "Macro with different parameter"
					" spelling already defined");
			CC__WARN("", idt->file, idt->line, idt->char_pos);
			CC__WARN("Previously defined here: ", 
				old->file, old->line, old->char_pos);
			CHECK(error);
		}
	}
	n = tk->first;
	o = old->body->first;

	while (n != 0 && 
		(n->type & CC_WHITE_SPACE) == CC_WHITE_SPACE) 
	{
		n = n->next;
	}	
	while (o != 0 && 
		(o->type & CC_WHITE_SPACE) == CC_WHITE_SPACE) 
	{
		o = o->next;
	}	
	while (n && o) {
		i = 0;
		while(n->identifier[i] != 0 &&
			n->identifier[i] == o->identifier[i]) 
		{
			i++;
		}
		if (n->type != o->type ||
			n->identifier[i] != o->identifier[i])
		{
			THROW(-100047, "Macro with different token sequence "
				"already defined");
			CC__WARN("", n->file, n->line, n->char_pos);
			CC__WARN("Previously defined here: ", 
				o->file, o->line, o->char_pos);
			CHECK(error);
		}
		n = n->next;
		o = o->next;
		if (n != 0 && o != 0 && (n->type & CC_WHITE_SPACE) !=
			(o->type & CC_WHITE_SPACE))
		{
			THROW(-100048, "Macro with different whitespace"
					" already defined");
			CC__WARN("", n->file, n->line, n->char_pos);
			CC__WARN("Previously defined here: ", 
				o->file, o->line, o->char_pos);
			CHECK(error);
		}
		while (n != 0 && 
			(n->type & CC_WHITE_SPACE) == CC_WHITE_SPACE) 
		{
			n = n->next;
		}	
		while (o != 0 && 
			(o->type & CC_WHITE_SPACE) == CC_WHITE_SPACE) 
		{
			o = o->next;
		}	
	}

	if (n != 0 || o != 0) {
		THROW(-100049, "Macro with different token sequence "
			"already defined");
		CC__WARN("", idt->file, idt->line, idt->char_pos);
		CC__WARN("Previously defined here: ", 
				old->file, old->line, old->char_pos);
		CHECK(error);	}
	return;
error:
	return;
}

static void clean_white_space(Ex **e__, cc__PreMacros *self, cc__PreTokens *tk)
{
	F("clean_white_space");
	cc__PreToken *t;

	(void)self;

	t = tk->first;

	while (t != 0 && (t->type & CC_WHITE_SPACE) == CC_WHITE_SPACE) {
		tk->_->delete_token(e__, tk, t);
		CHECK(error);
		t = tk->first;
	}

	t = tk->last;
	while (t != 0 && (t->type & CC_WHITE_SPACE) == CC_WHITE_SPACE) {
		tk->_->delete_token(e__, tk, t);
		CHECK(error);
		t = tk->last;
	}

	return;
error:
	return;
}


static void push_macro(Ex **e__, cc__PreMacros *self, cc__PreToken *identifier, 
	cc__PreTokens *tokens, cc__PreToken *parameters, cc__PreToken *start,
	int8 is_object)
{
	F("push_macro");
	utf8 **args = 0;
	int32 nb_args = 0;
	cc__PreTokens *tk = 0;
	cc__PreTokens *param = 0;
	utf8 *ptr = identifier->identifier;
	cc__PreMacro *old;
	cc__PreToken *t;

	old = self->_->get(e__, self, ptr);
	CHECK(error1);

	if (parameters != 0 && start != parameters) {
		if (start && start->previous) {
			t = start->previous;
		} else {
			t = tokens->last;
		}
		tokens->_->extract(e__, tokens, parameters, t, &param);
		CHECK(error0);
	}

	if (start != 0) {
		tokens->_->extract(e__, tokens, start, tokens->last, &tk);
		CHECK(error1);
	} else {
		tk = new_(cc__PreTokens);
		CHECK(error1);
	}
	if (param != 0) {
		int32 i = 1;
		cc__PreToken *n = param->first;

		while (n->next) {
			i++;
			n = n->next;
		}
	
		nb_args = i;
		args = (utf8**) Gc__malloc(e__, sizeof(utf8*) * i);
		CHECK(error2);
		n = parameters;
		i = 0;
		args[i] = n->identifier;
		n->identifier = Gc__malloc(e__, 1);
		CHECK(error3);
		n->identifier[0] = 0;
		while (n->next) {
			i++;
			n = n->next;
			args[i] = n->identifier;
			n->identifier = Gc__malloc(e__, 1);
			CHECK(error3);
			n->identifier[0] = 0;
		}
	}

	clean_white_space(e__, self, tk);
	CHECK(error3);

	if (old != 0) {
		check_equal_function(e__, self, old, args, nb_args,
			tk, is_object, identifier);
		CHECK(error3);
	} else {
		insert_function(e__, self, identifier,
			args, nb_args, tk, is_object);
		CHECK(error3);
	}
	if (param != 0) delete_(param);
	CHECK(error0);
	return;
error3:
	Gc__free(e__, args);
error2:
	if (tk != 0) delete_(tk);
error1:
	if (param != 0) delete_(param);
error0:
	return;
}

static void push_function(Ex **e__, cc__PreMacros *self,
	cc__PreToken *identifier, cc__PreTokens *tokens,
	cc__PreToken *parameters, cc__PreToken *start)
{
	F("push_function");

	push_macro(e__, self, identifier, tokens, parameters, start, 0);
	CHECK(error);
	
	return;
error:
	return;
}

static void push_object(Ex **e__, cc__PreMacros *self,
	cc__PreToken *identifier, cc__PreTokens *tokens, cc__PreToken *start)
{
	F("push_object");

	push_macro(e__, self, identifier, tokens, start, start, 1);
	CHECK(error);
	
	return;
error:
	return;
}

static int8 is_defined(Ex **e__, cc__PreMacros *self, utf8 *identifier)
{
	F("is_defined");
	cc__PreMacro *m;
	String s;

	STR(s, identifier);
	m = self->hash->_->get(e__, self->hash, &s);
	CHECK(error);
	if (m != 0) {
		return 1;
	}
	return 0;
error:
	return 0;
}

static cc__PreMacro *get(Ex **e__, cc__PreMacros *self, utf8 *identifier)
{
	F("get");
	cc__PreMacro *m;
	String s;

	STR(s, identifier);
	m = self->hash->_->get(e__, self->hash, &s);
	CHECK(error);
	return m;
error:
	return 0;
}

static void undef(Ex **e__, cc__PreMacros *self, cc__PreToken *id)
{
	F("undef");
	cc__PreMacro *m;
	String s;

	STR(s, id->identifier);
	self->hash->_->set(e__, self->hash, &s, 0, (void**)&m);
	CHECK(error);

	/* if (m == 0) {
		THROW(-1000XX, "Cannot undef macro, it is not defined.");
		CC__WARN("", id->file, id->line, id->char_pos);
		CHECK(error);
	} */
	if (m != 0) delete_(m);
	CHECK(error);
	return;
error:
	return;
}

static void set_env(Ex **e__, cc__PreMacros *self, cc__Env *env)
{
	F("set_env");

	(void)__func__;
	(void)e__;

	self->env = env;
	self->replaced->env = env;
	return;
}


static uint32 get_type_of_cstr(Ex **e__, cc__PreMacros *self,
	utf8 *cstr, int32 size)
{
	F("get_type_of_cstr");
	uint32 ret;

	ret = self->env->_->get_type_of_cstr(e__, self->env, cstr, size);
	CHECK(error);
	return ret;
error:
	return 0;
}

static void concatenate(Ex **e__, cc__PreMacros *self, cc__PreTokens *tokens,
	cc__PreToken *n, cc__PreToken *p)
{
	F("concatenate");
	String *s = 0;
	uint32 type;
	cc__PreTokens *tk;
	String quote;
	String single;
	String ps;
	String ns;
	uint32 line;
	uint32 file;
	uint32 char_pos;
	int32 rep_index;

	STR(single, "\'");
	STR(quote, "\"");
	STR(ps, p->identifier);
	STR(ns, n->identifier);
			
	s = new_(String);
	CHECK(error1);
	if (p->type != CC_PLACEMARKER) {
		if (p->type == CC_STRING) {
			s->_->add(e__, s, &quote);
			CHECK(error2);
		} else if (p->type == CC_CHARACTER) {
			s->_->add(e__, s,  &single);
			CHECK(error2);
		}
		s->_->add(e__, s, &ps);
		CHECK(error2);
		if (p->type == CC_STRING) {
			s->_->add(e__, s, &quote);
			CHECK(error2);
		} else if (p->type == CC_CHARACTER) {
			s->_->add(e__, s, &single);
			CHECK(error2);
		}
	}
	if (n->type != CC_PLACEMARKER) {
		if (n->type == CC_STRING) {
			s->_->add(e__, s, &quote);
			CHECK(error2);
		} else if (n->type == CC_CHARACTER) {
			s->_->add(e__, s, &single);
			CHECK(error2);
		}
		s->_->add(e__, s, &ns);
		CHECK(error2);
		if (n->type == CC_STRING) {
			s->_->add(e__, s, &quote);
			CHECK(error2);
		} else if (n->type == CC_CHARACTER) {
			s->_->add(e__, s, &single);
			CHECK(error2);
		}
		type = get_type_of_cstr(e__, self, s->cstr, s->size);
		CHECK(error2);
	} else if (p->type == CC_PLACEMARKER) {
		type = CC_PLACEMARKER;
	} else {
		type = get_type_of_cstr(e__, self, s->cstr, s->size);
		CHECK(error2);
	}

	rep_index = self->replaced->_->push(e__, self->replaced, 
		(utf8*)"", n->replace_index);
	CHECK(error2);
	self->replaced->_->add(e__, self->replaced, rep_index, p->replace_index);
	CHECK(error2);

	tokens->_->get_pos(e__, tokens, p, &file, &line, &char_pos);
	CHECK(error2);

	tk = new_(cc__PreTokens);
	CHECK(error2);
	tk->_->push(e__, tk, s->cstr, s->size, type, file, line, char_pos);
	CHECK(error3);
	tk->first->replace_index = rep_index;
	tokens->_->replace(e__, tokens, p, n, tk);
	CHECK(error3);
	delete_(s);
	CHECK(error1);
	return;
error3:
	delete_(tk);
error2:
	delete_(s);
error1:
	return;
}


/**
 * check if there is a ##
 */
static void concatenate_tokens(Ex **e__, cc__PreMacros *self,
	cc__PreTokens *tokens)
{
	F("concatenate_tokens");
	cc__PreToken *t;


	t = tokens->first;

	//6.10.3.3 The ## operator
	while (t != 0) {
		if (t->type == P('#', '#', 0, 0)) {
			cc__PreToken *n;
			cc__PreToken *p;
			cc__PreToken *next;
			cc__PreToken *prev;

			n = t->next;
			while (n != 0 && 
				(n->type & CC_WHITE_SPACE) == 
				CC_WHITE_SPACE)
			{
				n = n->next;
			}
			if (n == 0) {
				THROW(-100007, "'##' at end of "
					"replacement list");
				CC__WARN("", t->file, t->line, t->char_pos);
				CHECK(error1);
			}
			p = t->previous;
			while (p != 0 && 
				(p->type & CC_WHITE_SPACE) == 
				CC_WHITE_SPACE)
			{
				p = p->previous;
			}
			if (p == 0) {
				THROW(-100008, "'##' at begining of "
					"replacement list");
				CC__WARN("", t->file, t->line, t->char_pos);
				CHECK(error1);
			}
			next = n->next;
			prev = p->previous;
			concatenate(e__, self, tokens, n, p);
			CHECK(error1);
			t = next;
		} else {
			t = t->next;
		}
	}

	/* remove place makers */
	t = tokens->first;
	while (t != 0) {
		if (t->type == CC_PLACEMARKER) {
			cc__PreToken *n;
			
			n = t->next;
			tokens->_->delete_token(e__, tokens, t);
			CHECK(error1);
			t = n;
		} else {
			t = t->next;
		}
	}

	return;
error1:
	return;
}

static void set_line_def(Ex **e__, cc__PreMacros *self, 
	cc__PreToken *tk, uint32 line)
{
	F("set_line_def");
	String *s;
	String f;
	utf8 *old;

	(void)self;

	STR(f, "%d");
	s = new_(String);
	CHECK(error1);
	s->_->printf(e__, s, &f, line);
	CHECK(error2);
	old = tk->identifier;
	tk->identifier = s->cstr;
	tk->type = CC_DECIMAL;
	s->cstr = 0;
	s->size = 0;
	Gc__free(e__, old);
	CHECK(error2);
	delete_(s);
	CHECK(error1);
	return;
error2:
	delete_(s);
error1:
	return;
}

static void set_file_def(Ex **e__, cc__PreMacros *self, 
	cc__PreToken *tk)
{
	F("set_file_def");
	String *s;
	String *f;
	utf8 *old;

	f = self->env->include->_->get_current_file_full_path(e__, 
		self->env->include);
	CHECK(error1);

	s = new_(String);
	CHECK(error1);
	s->_->set(e__, s, f);
	CHECK(error2);
	old = tk->identifier;
	tk->identifier = s->cstr;
	tk->type = CC_STRING;
	s->cstr = 0;
	s->size = 0;
	Gc__free(e__, old);
	CHECK(error2);
	delete_(s);
	CHECK(error1);
	return;
error2:
	delete_(s);
error1:
	return;
}


static cc__PreToken *replace_macros_object(Ex **e__, cc__PreMacros *self, 
	cc__PreTokens *tokens, cc__PreToken *tk, cc__PreMacro *m, utf8 *name)
{
	F("replace_macros_object");
	cc__PreTokens *rep;
	cc__PreToken *ret;
	int32 rep_index;
	cc__PreToken *f;
	cc__PreTokens *body;

	body = m->body;

	if (body == self->file_def) {
		/* __FILE__ */
		set_file_def(e__, self, body->first);
		CHECK(error1);
	} else if (body == self->line_def) {
		/* __LINE__ */
		set_line_def(e__, self, body->first, tk->line);
		CHECK(error1);
	}

	ret = tk->previous;
	rep_index = self->replaced->_->push(e__, self->replaced, name, 
		tk->replace_index);
	CHECK(error1);
 	
	body->_->copy(e__, body, &rep);
	CHECK(error1);

	concatenate_tokens(e__, self, rep);
	CHECK(error2);

	f = rep->first;
	while (f != 0) {
		f->replace_index = rep_index;
		f = f->next;
	}

	tokens->_->replace(e__, tokens, tk, tk, rep);
	CHECK(error2);
	
	if (ret != 0) {
		tokens->next_to_expand_token = ret->next;
		ret = ret->next;
	} else {
		tokens->next_to_expand_token = tokens->first;
		ret = tokens->first;
	}
	return ret;
error2:
	delete_(rep);
error1:
	return 0;
}

static void free_arguments(Ex **e__, cc__PreMacros *self, cc__PreMacro *m,
	cc__PreTokens **parameters)
{
	F("free_arguments");
	int32 i;

	(void)self;

	if (m->nb_parameters == 0) {
		return;
	}	
	
	i = m->nb_parameters;
	while (i > 0) {
		i--;
		delete_(parameters[i]);
		CHECK(error1);
error1:
		continue;
	}
	Gc__free(e__, parameters);
	CHECK(error2);
	return;
error2:
	return;
}

static void set_argument(Ex **e__, cc__PreMacros *self, cc__PreTokens *arg,
	cc__PreTokens *tokens, cc__PreToken *begin, cc__PreToken *to)
{
	F("set_argument");

	(void)self;

	if (begin == to) {
		arg->_->push(e__, arg, (utf8*)"", 0, CC_PLACEMARKER, 
			to->file, to->line, to->char_pos);
		CHECK(error1);
	} else {
		if (tokens->next_to_execute_pragma == begin) {
			tokens->next_to_execute_pragma = to;
		}
		if (tokens->next_to_expand_token == begin) {
			tokens->next_to_expand_token = to;
		}
		arg->first = begin;
		arg->last = to->previous;

		to->previous = begin->previous;
		if (begin->previous != 0) {
			begin->previous->next = to;
		}
		arg->last->next = 0;
		arg->first->previous = 0;
		
	}
	return;
error1:
	return;
}


static cc__PreToken *get_arguments(Ex **e__, cc__PreMacros *self, 
	 cc__PreTokens *tokens, cc__PreToken *start, 
	cc__PreMacro *m, cc__PreTokens ***parameters)
{
	F("get_arguments");
	cc__PreToken *to;
	int32 nb_par = 1;
	int32 cur_arg = 0;
	cc__PreTokens **args;
	int32 nb_args = m->nb_parameters;
	int32 i;
	int8 last_is_white = 1;
	cc__PreToken *begin;
	int8 var_arg = 0;

	to = start;
/*	ECHO("***\n");
	while (to) {
		ECHO(to->identifier);
		to = to->next;
	}
	ECHO("***\n");
*/
	
	to = start->next;
	
	while (to != 0 && nb_par > 0) {
		if (to->type == P('(', 0, 0, 0)) {
			nb_par++;
			last_is_white = 0;
		} else if (to->type == P(')', 0, 0, 0)) {
			nb_par--;
			if (nb_par == 0) {
				break;
			}
		} else if (nb_par == 1) {
			if (to->type == P(',', 0, 0, 0)) {
				cur_arg++;
				last_is_white = 1;	
			} else if ((to->type & CC_WHITE_SPACE) != 
				CC_WHITE_SPACE) 
			{
				last_is_white = 0;
			}
		}
		to = to->next;
	}
	if (nb_par != 0) {
		/* we need more tokens */
		return 0;
	} 
	if (nb_args > 0) {
		if (compare_utf8(m->parameters[nb_args - 1], (utf8*)"...")
			== 0) 
		{
			var_arg = 1;
		}
	}
	if (last_is_white == 0) {
		cur_arg++;
	} else if (last_is_white == 1 && cur_arg < nb_args)
	{
		cur_arg++;
	}

	if (cur_arg > nb_args && var_arg == 0)
	{
		THROW(-100004, "Too many argument to macro");
		CC__WARN("", start->file, start->line, start->char_pos);
		CHECK(error1);
	} else if ((var_arg == 0 && cur_arg < nb_args) ||
		cur_arg < nb_args - 1) 
	{
		THROW(-100005, "Not enough argument to macro");
		CC__WARN("", start->file, start->line, start->char_pos);
		CHECK(error1);
	}
	if (nb_args == 0) {
		return to;
	}
	args = Gc__malloc(e__, nb_args * sizeof(*args));
	CHECK(error1);

	for (i = 0; i < nb_args; i++) {
		args[i] = new_(cc__PreTokens);
	}
	CHECK(error2);

	nb_par = 1;
	cur_arg = 0;
	begin = start->next;
/*	start->next = to->next;
	if (to->next) {
		to->next->previous = start;
	}
*/
	to = begin;

	while (to != 0 && nb_par > 0) {
		cc__PreToken *next = to->next;

		if (to->type == P('(', 0, 0, 0)) {
			nb_par++;
		} else if (to->type == P(')', 0, 0, 0)) {
			nb_par--;
			if (nb_par == 0) {
				set_argument(e__, self, args[cur_arg],
					tokens, begin, to);
				CHECK(error2);
				break;
			}
		} else if (nb_par == 1) {
			if (to->type == P(',', 0, 0, 0) && 
				(cur_arg < nb_args - 1 || var_arg == 0)) 
			{
				set_argument(e__, self, args[cur_arg],
					tokens, begin, to);
				CHECK(error2);
				cur_arg++;
				begin = next;
			}
		}
		to = next;
	}
	*parameters = args;
	return to;
error2:
	free_arguments(e__, self, m, args);
error1:
	return 0;
}


// 6.10.3.2 The # operator
static void stringize_tokens(Ex **e__, cc__PreMacros *self,
	cc__PreTokens *tokens)
{
	F("stringize_tokens");
	String *s;
	int32 i;
	cc__PreToken *t = tokens->first;
	String back_quote;
	String single_quote;
	String back;
	uint32 line = 0;
	uint32 file = 0;
	uint32 char_pos = 0;

	(void)self;

	STR(back_quote, "\\\"");
	STR(single_quote, "\'");
	STR(back, "\\");
	s = new_(String);
	CHECK(error1);
	while (t) {
		if (t->type == CC_STRING) {
			String txt;			
			utf8 *ptr1;
			utf8 *ptr2;

			STR(txt, t->identifier);
			ptr1 = ptr2 = t->identifier;
			s->_->add(e__, s, &back_quote);
			CHECK(error2);
			while (*ptr1) {
				if (*ptr2 == '\\') {
					if (ptr2 != ptr1) {
						String tmp;

						*ptr2 = 0;
						STR(tmp, ptr1);
						s->_->add(e__, s, &tmp);
						CHECK(error2);
						*ptr2 = '\\';
						ptr1 = ptr2;
					}
					s->_->add(e__, s, &back);
				} else if (*ptr2 == '\"') {
					if (ptr2 != ptr1) {
						String tmp;
						
						*ptr2 = 0;
						STR(tmp, ptr1);
						s->_->add(e__, s, &tmp);
						CHECK(error2);
						*ptr2 = '\"';
						ptr1 = ptr2;
					}
					s->_->add(e__, s, &back);
					CHECK(error2);
				} else if (*ptr2 == 0) {
					String tmp;

					STR(tmp, ptr1);
					s->_->add(e__, s, &tmp);
					CHECK(error2);
					ptr1 = ptr2;
				}
				ptr2++;
			}
			s->_->add(e__, s, &back_quote);
			CHECK(error2);
		} else if (t->type == CC_CHARACTER) {
			String txt;

			STR(txt, t->identifier);
			s->_->add(e__, s, &single_quote);
			CHECK(error2);
			if (t->identifier[0] == '\\') {
				s->_->add(e__, s, &back);
				CHECK(error2);
			}
			s->_->add(e__, s, &txt);
			CHECK(error2);
			s->_->add(e__, s, &single_quote);
			CHECK(error2);
		} else if ((t->type & CC_WHITE_SPACE) == CC_WHITE_SPACE) {
			if (s->size < 1 || 
				s->cstr[s->size - 1] != ' ')
			{
				String spc;

				STR(spc, " ");
				s->_->add(e__, s, &spc);
				CHECK(error2);
			}
		} else {
			String txt;

			STR(txt, t->identifier);
			s->_->add(e__, s, &txt);
			CHECK(error2);
		}
		if (t->next == 0) {
			tokens->_->get_pos(e__, tokens, t, &file, 
				&line, &char_pos);
			CHECK(error1);
		}
		t = t->next;
	}

	tokens->_->delete_end(e__, tokens, tokens->first);
	CHECK(error2);

	i = s->size;
	while (i > 0) {
		i--;
		if (IS_WHITE_SPACE(s->cstr[i]) == 0) {
			i++;
			s->cstr[i] = 0;
			break;
		}
	}

	s->size = i;

	i = 0;
	while (i < s->size) {
		if (IS_WHITE_SPACE(s->cstr[i]) == 0) {
			break;
		}
		i++;
	}
	tokens->_->push(e__, tokens, s->cstr + i, s->size - i, CC_STRING,
		file, line, char_pos);
	CHECK(error2);
	delete_(s);
	CHECK(error1);
	return;
error2:
	delete_(s);
error1:
	return;
}

static cc__PreToken *try_replace_parameter(Ex **e__, cc__PreMacros *self, 
	cc__PreTokens *tokens, cc__PreToken *stringize, cc__PreToken *f, 
	cc__PreTokens **parameters, cc__PreMacro *m)
{
	F("try_replace_parameter");
	int32 nb_arg = m->nb_parameters;
	utf8 **param = m->parameters;
	cc__PreTokens *rep = 0;
	cc__PreToken *ret = f->next;
/*	
	ECHO("\n=");
	ECHO(f->identifier);
*/

	if (nb_arg > 0 && compare_utf8(param[nb_arg - 1], (utf8*)"...") == 0) {
		nb_arg--;
		if (compare_utf8(f->identifier, (utf8*)"__VA_ARGS__") == 0) {
			parameters[nb_arg]->_->copy(e__, 
				parameters[nb_arg], &rep);
			CHECK(error1);
			if (stringize != 0) {
				stringize_tokens(e__, self, rep);
				CHECK(error2);
			} else if (rep->first == 0) {
				rep->_->push(e__, rep, (utf8*)"", 0, 
					CC_PLACEMARKER, f->file, f->line,
					f->char_pos);
				CHECK(error2);		
			}
		}
	}
	while (rep == 0 && nb_arg > 0) {
		nb_arg--;
		if (compare_utf8(f->identifier, param[nb_arg]) == 0) {
			parameters[nb_arg]->_->copy(e__, 
				parameters[nb_arg], &rep);
			CHECK(error1);
			if (stringize != 0) {
				stringize_tokens(e__, self, rep);
				CHECK(error2);
			} else if (rep->first == 0) {
				rep->_->push(e__, rep, (utf8*)"", 0, 
					CC_PLACEMARKER, f->file, f->line,
					f->char_pos);
				CHECK(error2);			
			}
			break;
		}
	}
	if (rep != 0) {
		if (stringize != 0) {
			tokens->_->replace(e__, tokens, stringize, f, rep);
			CHECK(error2);
		} else {
			cc__PreToken *t = rep->first;
			cc__PreToken *q = parameters[nb_arg]->first;
			
			while (t != 0) {
				t->replace_index = q->replace_index;
				t = t->next;
				q = q->next;
			}
			tokens->_->replace(e__, tokens, f, f, rep);
			CHECK(error2);
		}		
	}

	if (ret != 0) {
		ret = ret->previous;
	} else {
		ret = tokens->last;
	}

/*
	f = tokens->first;
	ECHO("=\n");
	while (f != 0) {
		ECHO(f->identifier);
		f = f->next;
	}
	ECHO("\n===\n");
*/
	return ret;
error2:
	delete_(rep);
error1:
	return 0;	
}

static void concatenate_parameters(Ex **e__, cc__PreMacros *self, 
	cc__PreTokens *tokens, cc__PreMacro *m, cc__PreTokens **parameters)
{
	F("concatenate_parameters");
	cc__PreToken *t;


	t = tokens->first;

	//6.10.3.3 The ## operator
	while (t != 0) {
		if (t->type == P('#', '#', 0, 0)) {
			cc__PreToken *n;
			cc__PreToken *p;
			cc__PreToken *next;
			cc__PreToken *chg;
			int8 go = 0;

			n = t->next;
			while (n != 0 && 
				(n->type & CC_WHITE_SPACE) == 
				CC_WHITE_SPACE)
			{
				n = n->next;
			}
			p = t->previous;
			while (p != 0 && 
				(p->type & CC_WHITE_SPACE) == 
				CC_WHITE_SPACE)
			{
				p = p->previous;
			}
			if (n != 0) {
				next = n->next;
			} else {
				next = t->next;
			}
			if (n != 0) {
				chg = try_replace_parameter(e__, self, tokens, 
						0, n, parameters, m);
				CHECK(error1);
				if (chg != n) {
					go = 1;
					n = t->next;
					while (n != 0 && 
						(n->type & CC_WHITE_SPACE) == 
						CC_WHITE_SPACE)
					{
						n = n->next;
					}
				}
			}
			if (p != 0) {
				chg = try_replace_parameter(e__, self, tokens, 
						0, p, parameters, m);
				CHECK(error1);
				if (chg != p) {
					go = 1;
					p = t->previous;
					while (p != 0 && 
						(p->type & CC_WHITE_SPACE) == 
						CC_WHITE_SPACE)
					{
						p = p->previous;
					}
				}	
			}
			if (go != 0 && p != 0 && n != 0) {
				concatenate(e__, self, tokens, n, p);
				CHECK(error1);
			}
			t = next;
		} else {
			t = t->next;
		}
	}
	return;
error1:
	return;
}

static cc__PreToken *replace_macros_func(Ex **e__, cc__PreMacros *self, 
	cc__PreTokens *tokens, cc__PreToken *tk, cc__PreMacro *m, utf8 *name)
{
	F("replace_macros_func");
	cc__PreTokens *rep;
	cc__PreToken *ret;
	int32 rep_index;
	int32 last_old = -2;
	int32 last_new = -2;
	cc__PreToken *f;
	cc__PreToken *to = tk;
	cc__PreTokens **parameters = 0;
	cc__PreToken *stringize = 0;
	int32 i;

	f = tk;
/*	ECHO("\n---\n");
	while (f != 0) {
		if (f->type == CC_PLACEMARKER) {
			ECHO("*PM*");
		} else {
			ECHO(f->identifier);
		}
		f = f->next;
	}
	ECHO("\n-=-\n");
*/
	/* check if we find the '(' after the macro name */
	f = tk->next;
	while (f != 0) {
		if ((f->type & CC_WHITE_SPACE) != CC_WHITE_SPACE) {
			break;
		}
		f = f->next;
	}
	if (f == 0) {
		/* we need more tokens from the file */
		tokens->next_to_expand_token = tk;
		return 0;
	}
	if (f->type != P('(', 0, 0, 0)) {
		/* this is not a function like macro invocation */
		tokens->next_to_expand_token = tk->next;
		return tk->next;
	}

	/* get the parameters of the function-like macro */
	to = get_arguments(e__, self, tokens, f, m, &parameters);
	CHECK(error1); 
	
	if (to == 0) {
		/* we need more tokens from the file, because we didn't
		 * reach the endding ')'. */
		tokens->next_to_expand_token = tk;
		return 0;
	}
	
	/* now add the macro name to the replaced indexes structure */
	ret = tk->previous;
	rep_index = self->replaced->_->push(e__, self->replaced,
		name, tk->replace_index);
	CHECK(error2);
 	
	/* get the raw replacement list */
	m->body->_->copy(e__, m->body, &rep);
	CHECK(error2);

	/* */
	concatenate_parameters(e__, self, rep, m, parameters);
	CHECK(error3);

	/* update the replacements indexes of the replacement list's tokens */
	f = rep->first;
	while (f != 0) {
		f->replace_index = rep_index;
		f = f->next;
	}

	/* add the parameters tokens' indexes to the replaced structure */
	f = tk->next;
	while (f != 0) {
		self->replaced->_->add(e__, self->replaced, rep_index, 
			f->replace_index);
		CHECK(error3);
		if (f == to) break;
		f = f->next;
	}

	/* update the replaced index of the parameters to add the index
	 * of the current macro name */
	i = m->nb_parameters;
	while (i > 0) {
		i--;
		expand_macros_or_args(e__, self, parameters[i], 0);
		CHECK(error3);

		f = parameters[i]->first;
		while (f != 0) {
			if (f->replace_index == -1) {
				f->replace_index = rep_index;
			} else if (f->replace_index == last_old) {
				f->replace_index = last_new;
			} else {				
				last_new = self->replaced->_->push(e__, 
					self->replaced, (utf8*)"", rep_index);
				CHECK(error3);
				self->replaced->_->add(e__, self->replaced,
					last_new, f->replace_index);
				CHECK(error3);
				last_old = f->replace_index;
				f->replace_index = last_new;
			}
			f = f->next;
		}
	}

	/* scan for parameter in the replacement list */
	f = rep->first;
	while (f != 0) {
		if (f->type == P('#', 0, 0, 0)) {
			stringize = f;
		} else if (f->type == CC_IDENTIFIER) {
			/* check if the token is a parameter, and replace
			 * it by its argument (or it's stringized argument) */
			f = try_replace_parameter(e__, self, rep, stringize,
				f, parameters, m);
			CHECK(error3);
			stringize = 0;
		} else if (stringize != 0 && 
			(f->type & CC_WHITE_SPACE) != CC_WHITE_SPACE)
		{
			THROW(15, "C compiler internal error.");
			CC__WARN("", f->file, f->line, f->char_pos);
			CHECK(error3);
		} 
		f = f->next;
	}
	if (stringize != 0) {
		THROW(15, "C compiler internal error.");
		CC__WARN("", f->file, f->line, f->char_pos);
		CHECK(error3);
	}
	concatenate_tokens(e__, self, rep);
	CHECK(error3);

	tokens->_->replace(e__, tokens, tk, to, rep);
	CHECK(error3);
	
	free_arguments(e__, self, m, parameters);
	CHECK(error2);

	if (ret != 0) {
		tokens->next_to_expand_token = ret->next;
		ret = ret->next;
	} else {
		tokens->next_to_expand_token = tokens->first;
		ret = tokens->first;
	}
	return ret;
error3:
	delete_(rep);
error2:
	free_arguments(e__, self, m, parameters);	
error1:
	return 0;
}



static void expand_macros_or_args(Ex **e__, cc__PreMacros *self, 
	cc__PreTokens *tokens, int8 is_arguments)
{
	F("expand_macros_or_args");
	cc__PreToken *tk;

	if (tokens->next_to_expand_token != 0) {
		tk = tokens->next_to_expand_token;
	} else {
		tk = tokens->first;
	}
	
	while (tk != 0) {
		tokens->next_to_expand_token = tk;
		if (tk->type == CC_IDENTIFIER) {

			if (self->replaced->_->is_already_replaced(
				e__, self->replaced, tk) == 0) 
			{
				cc__PreMacro *m;

				CHECK(error);
				m = get(e__, self, tk->identifier);
				CHECK(error);
				if (m != 0) {
					if (m->is_object != 0) {
						tk = replace_macros_object(
							e__, self, tokens, tk,
							m, tk->identifier);
						CHECK(error);
					} else {
						tk = replace_macros_func(
							e__, self, tokens, tk,
							m, tk->identifier);
						CHECK(error);
					}
				} else {
					if (is_arguments == 0) {
						tk->type = CC_EXPANDED_ID;
					}
					tk = tk->next;
				}
			} else {
				if (is_arguments == 0) {
					tk->type = CC_EXPANDED_ID;
				}
				tk = tk->next;
			}
		} else {
			tk = tk->next;
		}
	}
	
	return;
error:
	return;
}

EXPORT void cc__PreMacros__expand_macros(Ex **e__, cc__PreMacros *self, 
	cc__PreTokens *tokens)
{
	F("cc__PreMacros__expand_macros");
	
	expand_macros_or_args(e__, self, tokens, 0);
	CHECK(error);
	if (self->replaced->nb_replaced > 400 
		&& tokens->next_to_expand_token == tokens->last &&
		tokens->last->replace_index == -1)
	{
				/* we are processing non replaced tokens, 
				   so we don't need the list of replaced 
				   macro names anymore */
		self->replaced->_->clean(e__, self->replaced);
		CHECK(error);
	}

	return;
error:
	return;
}

INIT_EXPORT cc__PreMacros__class cc__PreMacros__  = {
	// Object
	(utf8*)"cc__PreMacros",
	sizeof(cc__PreMacros),
	0,
	__init__,
	__ctor__,
	__dtor__,
	hash_code,
	to_string,

	// PreMacros
	push_function,
	push_object,
	is_defined,
	undef,
	get,
	set_env,
	cc__PreMacros__expand_macros
};

