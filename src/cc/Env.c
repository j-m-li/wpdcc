/* "$Id: $"
 *
 *                          9 November MMVIII PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */


#include <cc/Env.h>
#include <cc/PreProcessor.h>
#include <pal/pal.h>

static void __init__(Ex **e__)
{
	(void)e__;
	cc__Env__.super = &Object__;
	return;
}


static void __ctor__(Ex **e__, cc__Env *self)
{
	F("__ctor__");

	self->include = new_(cc__PreIncludeFiles);
	CHECK(error1);

	self->lib_var = new_(Array);
	CHECK(error3);
	self->sources = new_(Array);
	CHECK(error4);
	self->define_var = new_(Array);
	CHECK(error5);

	self->only_preprocess = 0;
	self->add_line_directives = 1;

	self->buffer = new_(Buffer);
	CHECK(error6);
	self->last_file = 0xFFFFFFFF;
	self->last_line = 0xFFFFFFFF;

	self->out = 0;
	
	self->parser = new_(cc__Parser);
	CHECK(error7);
	self->parser->env = self;
	return;
error7:
	delete_(self->buffer);
error6:
	delete_(self->define_var);
error5:
	delete_(self->sources);
error4:
	delete_(self->lib_var);
error3:
//error2:
	delete_(self->include);
error1:
	return;
}

static void __dtor__(Ex **e__, cc__Env *self)
{
	F("__dtor__");

	(void)__func__;
	
	delete_(self->include);

	self->lib_var->_->clear_objects(e__, self->lib_var);
	delete_(self->lib_var);

	self->sources->_->clear_objects(e__, self->sources);
	delete_(self->sources);

	self->define_var->_->clear_objects(e__, self->define_var);
	delete_(self->define_var);

	delete_(self->buffer);
	return;
}

static uint32 hash_code(Ex **e__, cc__Env *self)
{
	(void)e__;
#ifdef _MSC_VER
	return *((uint32*)&self) >> 4;
#else 
	return (uint32)self >> 4;
#endif
} 

static void to_string(Ex **e__, cc__Env *self, String *out)
{
	F("to_string");
	String s;

	STR(s, self->_->__name__);
	out->_->add(e__, out, &s);
	CHECK(error);
error:
	return;
}

static void log_cmd(Ex **e__, cc__Env *self, utf8 *opt, utf8 *txt)
{
	F("log");
	Ex *e;
	String *s;
	String l;

	(void)self;

	STR(l, "'%s' %s");
	if (*e__ == 0) return;
	if ((*e__)->id >= 0) return;

	e = *e__;
	*e__ = 0;

	s = new_(String);
	CHECK(error);
	s->_->printf(e__, s, &l, opt, txt);
	CHECK(error);	
	e->_->log(e__, e, s);
	delete_(s);
	CHECK(error);
	*e__ = e;
	return;
error:
	delete_(e);
	return;
}

static utf8 *get_next_arg(Ex **e__, cc__Env *self, utf8 *start, String *out)
{
	F("get_next_arg");
	utf8 *ptr;
	int8 in_q = 0;
	String s;
	uint32 l;
	utf8 *sptr;
	utf8 *cstr;
	utf8 c;

	(void)self;
	
	while (*start == ' ' || *start == '\t') start++;
	ptr = start;
	while (*ptr != 0 && ((*ptr != ' ' && *ptr != '\t') || in_q != 0)) {
		if (*ptr == '"') {
			in_q = !in_q;
		}
		ptr++;
	}
	c = *ptr;
	*ptr = 0;
	STR(s, start);
	out->_->add(e__, out, &s);
	CHECK(error);
	*ptr = c;

	/* remove the '"' ... */
	cstr = sptr = out->cstr;
	l = 0;
	while (*sptr) {
		if (*sptr != '"') {
			cstr[l] = *sptr;
			l++;
		}
		sptr++;
	}
	cstr[l] = 0;
	out->size = l;
	return ptr;
error:
	return 0;
}

static utf8 *get_cmd_arg(Ex **e__, cc__Env *self, utf8 *start, utf8 *opt,
	String *out)
{
	F("get_cmd_arg");
	utf8 *ptr;

	ptr = get_next_arg(e__, self, start, out);
	CHECK(error);
	if (out->size == 0) {
		THROW(-100100, "Command line error.");
		log_cmd(e__, self, opt, (utf8*)"requires an argument.");
		CHECK(error);
	}
	return ptr;
error:
	return 0;
}



static void add_define(Ex **e__, cc__Env *self, String *def)
{
	F("add_define");
	Array *a;
	String *s;
	uint32 index;
	String format;
	utf8 *cstr;

	STR(format, "#define %Z\r\n");
	cstr = def->cstr;
	while (*cstr) {
		if (*cstr == '=') {
			*cstr = ' ';
			break;
		}
		cstr++;
	}
	s = new_(String);
	CHECK(error1);
	s->_->printf(e__, s, &format, def);
	a = self->define_var;
	index = a->_->get_length(e__, a);
	CHECK(error2);
	a->_->set(e__, a, index, s);
	CHECK(error2);
	return;
error2:
	delete_(s);
error1:
	return;
}

static utf8 *get_opt(Ex **e__, cc__Env *self, utf8 *ptr2, String *s)
{
	F("get_opt");

	s->_->clear(e__, s);
	CHECK(error);
	ptr2++;
	switch (*ptr2) {
	case 'I':
		ptr2++;
		ptr2 = get_cmd_arg(e__, self, ptr2, (utf8*)"/I", s);
		CHECK(error);
		self->include->_->add_include_path(e__, self->include, s);
		CHECK(error);
		break;
	case 'D':
		ptr2++;
		ptr2 = get_cmd_arg(e__, self, ptr2, (utf8*)"/D", s);
		CHECK(error);
		add_define(e__, self, s);
		CHECK(error);
		break;
	case 'P':
		ptr2++;
		self->only_preprocess = 1;
		break;

	}
	return ptr2;
error:
	return 0;
}

static utf8 *add_source_file(Ex **e__, cc__Env *self, utf8 *start, String *out)
{
	F("add_source_file");
	Array *a;
	String *s;
	uint32 index;
	
	out->_->clear(e__, out);
	CHECK(error1);
	start = get_next_arg(e__, self, start, out);
	CHECK(error1);
	if (out->size == 0) return start;

	s = new_(String);
	CHECK(error1);
	s->_->set(e__, s, out);
	CHECK(error2);
	a = self->sources;
	index = a->_->get_length(e__, a);
	CHECK(error2);
	a->_->set(e__, a, index, s);
	CHECK(error2);
	return start;
error2:
	delete_(s);
error1:
	return 0;
}
 
static void init_cmd(Ex **e__, cc__Env *self)
{
	F("init_cmd");
	utf8 *ptr2;
	uint8 in_q = 0;
	String *s;

	s = new_(String);
	CHECK(error1);
	
	ptr2 = GetCommandLineU();
	while (*ptr2) {
		if (*ptr2 == '"') in_q = !in_q;
		if (in_q == 0 && (*ptr2 == ' ' || *ptr2 == '\t')) {
			ptr2++;
			break;
		}
		ptr2++;
	}
	while (*ptr2) {
		if (*ptr2 == '/') {
			ptr2 = get_opt(e__, self, ptr2, s);
			CHECK(error2);
			continue;
		} else if (*ptr2 != ' ' && *ptr2 != '\t') {
			ptr2 = add_source_file(e__, self, ptr2, s);
			CHECK(error2);
			continue;
		} 
		ptr2++;
	}
	delete_(s);
	return;
error2:
	delete_(s);
error1:
	return;
}

static void init(Ex **e__, cc__Env *self)
{
	F("init");
	String d;
	String t;
	String q;
	String stdc;
	String hosted;
	String version;
	String iec;
	String neq;
	String complex;
	String iso;
	String *s;
	utf8 buf[100];

	STR(d, "__DATE__=\"");
	STR(t, "__TIME__=\"");
	STR(q, "\"");
	STR(stdc, "__STDC__=1");
	STR(hosted, "__STDC_HOSTED__=1");
	STR(version, "__STDC_VERSION__=199901L");
	STR(iec, "__STDC_IEC_559__=1");
	STR(neq, "__STDC_MB_MIGHT_NEQ_EC__=1");
	STR(complex, "__STDC_IEC_559_COMPLEX__=1");
	STR(iso, "__STDC_ISO_10646__=199712L");

	init_cmd(e__, self);	
	self->include->_->init_include_var(e__, self->include, self);
	CHECK(error1);
	
	s = new_(String);
	s->_->add(e__, s, &d);	
	GetDateFormatU(LOCALE_INVARIANT, 0, 0, (utf8*)"MMM d yyyy", buf, 100);
	d.cstr = buf;
	s->_->add(e__, s, &d);
	s->_->add(e__, s, &q);	
	add_define(e__, self, s);

	s->_->set(e__, s, &t);	
	GetTimeFormatU(LOCALE_INVARIANT, 0, 0, 0, buf, 100);
	d.cstr = buf;
	s->_->add(e__, s, &d);
	s->_->add(e__, s, &q);	
	add_define(e__, self, s);

	s->_->set(e__, s, &iso);
	add_define(e__, self, s);
	s->_->set(e__, s, &stdc);
	add_define(e__, self, s);
	s->_->set(e__, s, &hosted);
	add_define(e__, self, s);
	s->_->set(e__, s, &version);
	add_define(e__, self, s);
	s->_->set(e__, s, &iec);
	add_define(e__, self, s);
	s->_->set(e__, s, &neq);
	add_define(e__, self, s);
	s->_->set(e__, s, &complex);
	add_define(e__, self, s);

	delete_(s);
	return;
error1:
	return;
}


static void log_(Ex **e__, cc__Env *self, String *s1, uint32 file, uint32 line,
		uint32 char_pos)
{
	F("log");
	Ex *e;
	String *s;

	if (*e__ == 0) return;
	if ((*e__)->id >= 0 && (*e__)->id != 10) return;

	e = *e__;
	*e__ = 0;

	s = new_(String);
	CHECK(error);
	
	self->include->_->get_include_tree(e__, self->include, s, file, line);
	CHECK(error);

	s->_->add(e__, s, s1); 
	self->include->_->get_line(e__, self->include, s, file, line, char_pos);
	e->_->log(e__, e, s);
	//echo(s);
	delete_(s);
	CHECK(error);
	*e__ = e;
	return;
error:
	delete_(e);
	return;
}



static void c_preprocess(Ex **e__, cc__Env *self, String *s)
{
	F("c_preprocess");
	cc__PreProcessor *pre;
	Array *a;
	uint32 i;

	self->preprocessor = pre = new_(cc__PreProcessor);
	CHECK(error1);
	pre->_->set_env(e__, pre, self);
	CHECK(error2);

	a = self->define_var;
	i = a->_->get_length(e__, a);
	CHECK(error2);
	while (i > 0) {
		String *s1;
		i--;
		s1 = a->_->get(e__, a, i);
		CHECK(error2);
		pre->_->set(e__, pre, s1);
		CHECK(error2);
		pre->_->process(e__, pre);
		CHECK(error2);
	}	
	pre->_->clear(e__, pre);
	CHECK(error2);
	self->include->_->clear(e__, self->include);
	CHECK(error2);
	self->last_file = 0xFFFFFFFF;
	self->last_line = 0xFFFFFFFF;
	self->add_line_directives = 1;

	if (self->only_preprocess != 0) {
		String mode;
		io__File *out;

		utf8 *cstr;
		uint32 i;
		utf8 c[2];
		uint32 si;
		
		STR(mode, "write");
		out = new_(io__File);
		CHECK(error2);
		self->out = out;
		cstr = s->cstr;
		si = i = s->size;
		while (i > 0) {
			i--;
			if (cstr[i] == '.') break;
		}
		if (i == 0 || i >= si - 1) {
			THROW(-100101, "Invalid source file name.");
			CHECK(error3);
		} 
		i++;
		c[0] = cstr[i];
		c[1] = cstr[i + 1];
		cstr[i] = 'i';
		cstr[i + 1] = 0;
		s->size = i + 1;
		out->_->open(e__, out, s, &mode);
		s->size = si;
		cstr[i] = c[0];
		cstr[i + 1] = c[1];
		CHECK(error3);
		out->_->clear(e__, out);
		CHECK(error3);
	}

	pre->_->process_file(e__, pre, s);
	CHECK(error3);
	pre->_->run(e__, pre);
	CHECK(error3);
	
	if (self->only_preprocess != 0) {
		Buffer *b = self->buffer;

		self->out->_->add(e__, self->out, (String*)b);
		CHECK(error2);
		b->_->clear(e__, b);
		CHECK(error2);
		delete_(self->out);
		CHECK(error2);
	}
	delete_(pre);
	CHECK(error1);		
	return;
error3:
	delete_(self->out);
error2:
	delete_(pre);
error1:
	return;
}

static void run(Ex **e__, cc__Env *self)
{
	F("run");
	Array *a;
	String *s;
	uint32 i, l;

	a = self->sources;
	l = a->_->get_length(e__, a);
	CHECK(error);
	for (i = 0; i < l; i++) {
		s = a->_->get(e__, a, i);
		CHECK(error);
		c_preprocess(e__, self, s);
		CHECK(error);
	}
	return;
error:
	return;
}


static void escape_preprocess_string(Ex **e__, utf8 *start, Buffer *b)
{
	F("escape_preprocess_string");
	utf8 *ptr = start;

	while (*ptr) {
		switch (*ptr) {
		case '\n':
		case '\r':
		case '\'':
		case '"':
		case '\\':
			if (ptr != start) {
				uint32 *p = (uint32*)&ptr;
				uint32 *s = (uint32*)&start;

				b->_->add_data(e__, b, start, *p - *s);
			}
			
			if (*ptr == '\n') {
				ptr++;
				b->_->add_data(e__, b, (utf8*)"\\n", 2);
			} else if (*ptr == '\r') {
				ptr++;
				b->_->add_data(e__, b, (utf8*)"\\r", 2);

			} else {
				b->_->add_data(e__, b, (utf8*)"\\", 1);
			}
			start = ptr;
			break;
		}
		ptr++;
	}
	if (ptr != start) {
		uint32 *p = (uint32*)&ptr;
		uint32 *s = (uint32*)&start;
		b->_->add_data(e__, b, start, *p - *s);
	}

	CHECK(error);
	return;
error:
	return;
}

static void print_line(Ex **e__, cc__Env *self, Buffer *b, uint32 line,
	uint32 file)
{
	String *fi;
	String *s;
	String f;

	(void)file;
	STR(f, "\r\n#line %d \"");
	//file; // FIXME;
	fi = self->include->_->get_current_file_full_path(e__, self->include);

	if (fi == 0) {
		return;
	}
	s = new_(String);
	s->_->printf(e__, s, &f, line);
	b->_->add(e__, b, s);
	delete_(s);
		
	escape_preprocess_string(e__, fi->cstr, b);
	b->_->add_data(e__, b, (utf8*)"\"\r\n", 3);
	return;
}

static void write_preprocess_tokens(Ex **e__, cc__Env *self,
	cc__PreProcessor *pre)
{
	F("write_preprocess_tokens");
	cc__PreToken *tk;
	Buffer *b = self->buffer;

	tk = pre->tokens->first;
	
	while (tk != 0 && tk != pre->tokens->next_to_expand_token) {
		int32 l = 0;
		utf8 *p = tk->identifier;
		
		//ECHO(p);
	
		if (self->add_line_directives != 0 &&
			((tk->type & CC_WHITE_SPACE) != CC_WHITE_SPACE ||
			self->last_file == 0xFFFFFFFF)) 
		{
			if (
				tk->line > self->last_line + 1 ||
				tk->file != self->last_file)
			{
				print_line(e__, self, b, tk->line, tk->file);
				CHECK(error);
			}
			self->last_file = tk->file;

		}
		self->last_line = tk->line;

		if (p == 0) p = (utf8*)"";
		while (p[l] != 0) l++; 
		if (tk->type == CC_STRING) {
			b->_->add_data(e__, b, (utf8*)"\"", 1);
			b->_->add_data(e__, b, p, l);
			b->_->add_data(e__, b, (utf8*)"\"", 1);
			CHECK(error);
		} else if (tk->type == CC_CHARACTER) {
			b->_->add_data(e__, b, (utf8*)"\'", 1);
			b->_->add_data(e__, b, p, l);
			b->_->add_data(e__, b, (utf8*)"\'", 1);
			CHECK(error);
		} else if (tk->type == CC_PRAGMA) {
			b->_->add_data(e__, b, (utf8*)"_Pragma(\"", 9);
			b->_->add_data(e__, b, p, l);
			b->_->add_data(e__, b, (utf8*)"\")", 2);
			CHECK(error);
		} else if (tk->type == CC_LINE) {
			self->add_line_directives = 0;
			b->_->add_data(e__, b, (utf8*)"\r\n#line ", 8);
			b->_->add_data(e__, b, p, l);
			if (tk->next->type == CC_FILE) {
				pre->tokens->_->delete_token(e__, 
					pre->tokens, tk);

				tk = pre->tokens->first;
				p = tk->identifier;
				while (p[l] != 0) l++;
				b->_->add_data(e__, b, (utf8*)" \"", 2);
				b->_->add_data(e__, b, p, l);
				b->_->add_data(e__, b, (utf8*)"\"", 1); 
			}
			b->_->add_data(e__, b, (utf8*)"\r\n", 2);
			CHECK(error);
		} else if (tk->type == CC_END_TOKEN) {			
		} else {
			b->_->add_data(e__, b, tk->identifier, l);
			CHECK(error);
		}
		pre->tokens->_->delete_token(e__, pre->tokens, tk);
		tk = pre->tokens->first;
		CHECK(error);
		if (tk == pre->tokens->last && tk->type == CC_END_TOKEN) {
			break;
		}
	}

	if (b->size >= 2048) {
		self->out->_->add(e__, self->out, (String*)b);
		CHECK(error);
		b->_->clear(e__, b);
		CHECK(error);
	}
	return;
error:
	return;
}

static void parse(Ex **e__, cc__Env *self)
{
	F("parse");
	cc__PreProcessor *pre;

	pre = self->preprocessor;
	if (pre->tokens->next_to_execute_pragma != pre->tokens->last) {
		return;
	}
	if (self->only_preprocess != 0) {
		write_preprocess_tokens(e__, self, pre);
		CHECK(error);
		return;
	}
	self->parser->_->parse(e__, self->parser, pre->tokens);
	CHECK(error);
	return;
error:
	return;
}

static uint32 get_type_of_cstr(Ex **e__, cc__Env *self, utf8 *cstr, int32 size)
{
	F("get_type_of_cstr");
	uint32 r;

	r = self->preprocessor->_->get_type_of_cstr(e__, 
		self->preprocessor, cstr, size);
	CHECK(error);
	return r;
error:
	return 0; 
}


INIT_EXPORT cc__Env__class cc__Env__  = {
	// Object
	(utf8*)"cc__Env",
	sizeof(cc__Env),
	0,
	__init__,
	__ctor__,
	__dtor__,
	hash_code,
	to_string,

	// Env
	init,
	log_,
	run,
	parse,
	get_type_of_cstr
};

