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

	cc__PreIncludeFiles__.super = &Object__;
	return;
}


static void __ctor__(Ex **e__, cc__PreIncludeFiles *self)
{
	F("__ctor__");

	self->current_file_id = 0;
	self->files_name = new_(Array);
	CHECK(error1);
	self->include_tree = 0;
	self->nb_tree = 0;
	self->ma_tree = 0;
	
	self->include_var = new_(Array);
	CHECK(error2);

	self->files = new_(Stack);
	CHECK(error3);

	self->once = new_(Array);
	CHECK(error4);

	self->condition_stack = 0;
	self->alloced_condition = 0;
	self->nb_condition = 0;
	self->condition = 1;

	return;
error4:
	delete_(self->files);
error3:
	delete_(self->include_var);
error2:
	delete_(self->files_name);
error1:
	return;
}

static void __dtor__(Ex **e__, cc__PreIncludeFiles *self)
{
	F("__dtor__");
	
	self->files_name->_->clear_objects(e__, self->files_name);
	delete_(self->files_name);

	if (self->include_tree != 0) {
		Gc__free(e__, self->include_tree);
	}
	self->include_var->_->clear_objects(e__, self->include_var);
	delete_(self->include_var);
	delete_(self->files);
	self->once->_->clear_objects(e__, self->once);
	delete_(self->once);
	Gc__free(e__, self->condition_stack);
	CHECK(error2);
error2:

	return;
}

static uint32 hash_code(Ex **e__, cc__PreIncludeFiles *self)
{
	(void)e__;
#ifdef _MSC_VER
	return *((uint32*)&self) >> 4;
#else 
	return (uint32)self >> 4;
#endif
}
 
static void to_string(Ex **e__, cc__PreIncludeFiles *self, String *out)
{
	F("to_string");
	String s;

	STR(s, self->_->__name__);
	out->_->add(e__, out, &s);
	CHECK(error);
error:
	return;
}

static void clear(Ex **e__, cc__PreIncludeFiles *self)
{
	self->current_file_id = 0;
	self->files_name->_->clear_objects(e__, self->files_name);

	Gc__free(e__, self->include_tree);
	self->ma_tree = 0;
	self->nb_tree = 0;
	self->include_tree = 0;
	self->files->nb_data = 0;
	self->once->_->clear_objects(e__, self->once);
	Gc__free(e__, self->condition_stack);
}

static void add_include_path(Ex **e__, cc__PreIncludeFiles *self, String *path)
{
	F("add_include_path");
	Array *a;
	String *s;
	uint32 index;
	String sl;

	STR(sl, "/");
	s = new_(String);
	CHECK(error1);
	s->_->set(e__, s, path);
	s->_->add(e__, s, &sl);
	CHECK(error2);
	a = self->include_var;
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

static void init_include_var(Ex **e__, cc__PreIncludeFiles *self, cc__Env *env)
{
	F("init_include_var");
	Array *a;
	uint32 r, r2;
	utf8 *buf;
	utf8 *ptr2;

	self->env = env;
	a = self->include_var;

	r = GetEnvironmentVariableU((utf8*)"INCLUDE", 0, 0);
	if (r == 0) return;
	buf = Gc__malloc(e__, r);
	CHECK(error1);
	r2 = GetEnvironmentVariableU((utf8*)"INCLUDE", buf, r);
	if (r2 < r) {
		String s;
		ptr2 = buf;
		STR(s, buf);
		while (*ptr2) {
			if (*ptr2 == ';') {
				*ptr2 = 0;
				add_include_path(e__, self, &s);
				CHECK(error2);
				s.cstr = ptr2 + 1;	
			}
			ptr2++;
		}
		if (s.cstr[0] != 0) {
			add_include_path(e__, self, &s);
			CHECK(error2);
		}
	}

	Gc__free(e__, buf);
	CHECK(error1);
	return;
error2:
	Gc__free(e__, buf);
error1:
	return;
}


static void get_line(Ex **e__, cc__PreIncludeFiles *self, String *s,
	uint32 file, uint32 line, uint32 char_pos)
{
	F("get_line");
	String *name;
	io__File *f;
	String mode;
	uint32 file_line;
	String *sl;
	String shs;
	String nl;
	utf8 show[] = 	"          " "          " "          " "          "
			"          " "          " "          " "         "

			"\n" 
			"          " "          " "          " "          "
			"          " "          " "          " "         "
			"\n";
	uint32 i;
	uint32 op = char_pos % 75;
	int32 o;
	utf8 *cstr;
	int32 size;

	STR(mode, "share,exist");
	STR(nl, "\r\n");
	STR(shs, show);
	name = self->_->get_file_path(e__, self, file);
	CHECK(error99);
	if (name == 0) return;

	s->_->add(e__, s, &nl);
	CHECK(error99);

	f = new_(io__File);
	CHECK(error99);
	f->_->open(e__, f, name, &mode);
	CHECK(error1);

	sl = new_(String);
	CHECK(error1);
	while (f->_->end_of_file(e__, f) == 0) {
		CHECK(error2);
		f->_->read_line(e__, f, sl);
		CHECK(error2);
		file_line = f->_->get_line_number(e__, f);
		CHECK(error2);
		if (file_line == line) break;
		sl->_->clear(e__, sl);
		CHECK(error2);
	}

	cstr = sl->cstr;
	size  = sl->size;
	o = char_pos - op;
	if (size == 0) o = 0; 
	for (i = 0; i < 79 && o < size; i++, o++) {
		if (cstr[o] == '\n' || cstr[o] == '\r') break;
		if (cstr[o] == '\t') {
			show[i] = ' ';
		} else {
			show[i] = cstr[o];
		}
	}
	show[op + 80] = '^';

	s->_->add(e__, s, &shs);
	CHECK(error2);

	delete_(sl);
	delete_(f);
	return;
error2:
	delete_(s);
error1:
	delete_(f);
error99:
	return;
}

static void get_include_tree(Ex **e__, cc__PreIncludeFiles *self, String *s,
	uint32 file, uint32 line)
{
	F("get_include_tree");
	String l;
	String l2;
	uint32 fn;
	String *sfn;
	String *format = &l2;

	STR(l, "\n%Z(%d) : ");
	STR(l2, "%Z(%d) : ");
	if (file >= self->nb_tree) return;

	fn = self->include_tree[file].file;

	if (file != 0) {
		get_include_tree(e__, self, s, 
			self->include_tree[file].parent, 
			self->include_tree[file].parent_line);
		CHECK(error99);
		format = &l;
	}
	sfn = self->files_name->_->get(e__, self->files_name, fn);
	CHECK(error99);
	s->_->printf(e__, s, format, sfn, line);
	CHECK(error99);
	return;
error99:
	return;
}

static void get_previous_def(Ex **e__, cc__PreIncludeFiles *self, String *s,
	uint32 file, uint32 line)
{
	F("get_previous_def");
	String l;
	uint32 fn;

	STR(l, "Previous definition here:\n%Z(%d) :\n");
	fn = self->include_tree[file].file;
	s->_->printf(e__, s, &l, self->files_name[fn], line);
	CHECK(error99);
	return;
error99:
	return;
}



static void push_tree(Ex **e__, cc__PreIncludeFiles *self,
	uint32 file, uint32 parent_line)
{
	F("push_tree");
	cc__PreIncludeFiles_tree *it;
	uint32 nbt;
	uint32 i;

	if (self->nb_tree >= self->ma_tree) {
		cc__PreIncludeFiles_tree *na;

		nbt = self->nb_tree;
		it = self->include_tree;
		self->ma_tree += 512;
		na = Gc__malloc(e__, sizeof(cc__PreIncludeFiles_tree)
			* self->ma_tree);
		CHECK(error99);
		for (i = 0; i < nbt; i++) {
			na[i] = it[i];
		}
		self->include_tree = na;
		if (it != 0) {		
			Gc__free(e__, it);
			CHECK(error99);
		}
	}

	self->include_tree[self->nb_tree].parent = self->current_file_id;
	self->include_tree[self->nb_tree].file = file;
	self->include_tree[self->nb_tree].parent_line = parent_line;
	self->include_tree[self->nb_tree].condition_stack = 
		self->condition_stack;
	self->include_tree[self->nb_tree].alloced_condition = 
		self->alloced_condition;
	self->include_tree[self->nb_tree].nb_condition = self->nb_condition;

	self->condition_stack = 0;
	self->alloced_condition = 0;
	self->nb_condition = 0;
	self->condition = 1;

	if (self->current_file_id > 0) {
		uint32 d;

		d = self->include_tree[self->nb_tree].depth =
			self->include_tree[self->current_file_id].depth + 1;
		if (d > 1000) {
			THROW(-100035, "Too many include depth.");
			CC__WARN("", self->current_file_id, parent_line, 0);
			CHECK(error99);
		}
	} else {
		self->include_tree[self->nb_tree].depth = 0;
	}
	self->current_file_id = self->nb_tree;
	self->nb_tree++;
	return;
error99:
	return;
}

static void set_current_file(Ex **e__, cc__PreIncludeFiles *self, String *name, 
	uint32 parent_line)
{
	F("set_current_file");
	int32 len;
	uint32 i;
	uint32 nbf;
	Array *fn;
	utf8 *cstr;
	String *s;

	len = name->_->get_size(e__, name);
	CHECK(error99);
	cstr = name->_->get_cstr(e__, name);
	CHECK(error99);
	fn = self->files_name;
	nbf = fn->_->get_length(e__, fn);
	CHECK(error99);
	
	for (i = 0; i < nbf; i++) {
		String *fs;
				
		fs = fn->_->get(e__, fn, i);
		CHECK(error99);
		if (fs->size == len) {
			int32 j = len;
			utf8 *c = fs->cstr;
	
			while (j > 0) {
				j--;
				if (c[j] != cstr[j]) break;
			} 			
			if (j == 0) {
				push_tree(e__, self, i, parent_line);
				CHECK(error99);
				return;
			}
		}	
	}
	

	push_tree(e__, self, nbf, parent_line);
	CHECK(error99);

	s = new_(String);
	CHECK(error99);
	s->_->set(e__, s, name);
	CHECK(error1);
	fn->_->set(e__, fn, nbf, s);
	CHECK(error99);
	return;
error1:
	delete_(s);
error99:
	return;
}

static String *get_current_file_full_path(Ex **e__, cc__PreIncludeFiles *self)
{
	String *s;

	s = self->_->get_file_path(e__, self, self->current_file_id); 
	return s;
}

static void revert_file(Ex **e__, cc__PreIncludeFiles *self)
{
	F("revert_file");
	uint32 i = self->current_file_id;

	if (i == 0) {
		return;
	}
	Gc__free(e__, self->condition_stack);
	CHECK(error);
	self->condition_stack = self->include_tree[i].condition_stack;
	self->alloced_condition = self->include_tree[i].alloced_condition;
	self->nb_condition = self->include_tree[i].nb_condition;
	self->current_file_id = self->include_tree[i].parent;
	self->condition = 1;

	return;
error:
	return;
}

static void get_include_local_file(Ex **e__, cc__PreIncludeFiles *self,
	String *name, String *out, String *current)
{
	F("get_include_local_file");
	String *s;
	String sl;

	(void)self;

	STR(sl, "/");
	s = new_(String);
	CHECK(error1);
	s->_->set(e__, s, current);
	CHECK(error2);
	s->_->add(e__, s, &sl);
	s->_->add(e__, s, name);
	CHECK(error2);

	if (INVALID_FILE_ATTRIBUTES != GetFileAttributesU(s->cstr)) {
		out->_->add(e__, out, s);
		CHECK(error2);
	}
	delete_(s);
	return;
error2:
	delete_(s);
error1:
	return;
}

static void get_include_file(Ex **e__, cc__PreIncludeFiles *self, String *name, 
	String *out, String *current)
{
	F("get_include_file");
	Array *a;
	uint32 l;
	uint32 i = 0;
	String *s;
	String sl;

	STR(sl, "/");

	if (current != 0) {
		get_include_local_file(e__, self, name, out, current);
		CHECK(error1);
		if (out->_->get_size(e__, out) > 0) return;
	}

	a = self->include_var;
	l = a->_->get_length(e__, a);
	CHECK(error1);

	s = new_(String);
	while (i < l) {
		String *p;
		
		p = a->_->get(e__, a, i);
		CHECK(error2);
		s->_->set(e__, s, p);
		CHECK(error2);
		s->_->add(e__, s, &sl);
		s->_->add(e__, s, name);
		CHECK(error2);
		i++;
		if (INVALID_FILE_ATTRIBUTES != GetFileAttributesU(s->cstr)) {
			out->_->add(e__, out, s);
			CHECK(error2);
			break;
		}
	}
	delete_(s);
	return;
error2:
	delete_(s);
error1:
	return;
}

static String *get_file_path(Ex **e__, cc__PreIncludeFiles *self, uint32 file)
{
	F("get_file_path");
	String *out;
	uint32 fn;

	if (file >= self->nb_tree) return 0;
	fn = self->include_tree[file].file;
	out = self->files_name->_->get(e__, self->files_name, fn);
	CHECK(error99);
	return out;
error99:
	return 0;
	
}

static io__File *open_file(Ex **e__, cc__PreIncludeFiles *self, String *name)
{
	F("open_file");
	io__File *f;
	String mode;
	String *s;
	uint32 l;
	String *fp;
	Array *once = self->once;

	STR(mode, "share,exist");
	f = new_(io__File);
	CHECK(error1);
	f->_->open(e__, f, name, &mode);
	CHECK(error2);
	
	fp = f->full_path_name;
	l = self->once->_->get_length(e__, self->once);
	CHECK(error2);
	while (l > 0) {
		int8 c;			
		IComparable ic;

		l--;
		s = once->_->get(e__, self->once, l);
		CHECK(error2);
		s->_->i_comparable(e__, s, &ic);
		c = ic._->compare_to(e__, &ic, (Object*)fp);
		CHECK(error2);
		if (c == 0) {
			delete_(f);
			CHECK(error1);
			return 0;
		}		
	}
	return f;
error2:
	delete_(f);
error1:
	return 0;
}

static void include_once(Ex **e__, cc__PreIncludeFiles *self)
{
	F("get_file_path");
	String *s;
	uint32 l;
	io__File *f;
	
	s = new_(String);
	CHECK(error1);
	self->files->_->pop(e__, self->files, (void**)&f);
	self->files->_->push(e__, self->files, f);
	CHECK(error2);
	f->_->to_string(e__, f, s);
	CHECK(error2);
	l = self->once->_->get_length(e__, self->once);
	CHECK(error2);
	self->once->_->set(e__, self->once, l, s);
	CHECK(error2);
	return;
error2:
	delete_(s);
error1:
	return;
}

/**
 * push the current "condition" on the stack and set the condition to "value".
 * "condition" is the value used to either skip some part of the file or to
 * process them.
 */
static void push_condition(Ex **e__, cc__PreIncludeFiles *self, int8 value)
{
	F("push_condition");

	if (self->alloced_condition >= self->nb_condition) {
		int8 *nc;
		int8 *oc = self->condition_stack;
		int32 ns = self->alloced_condition + 128;
		int32 i;
		
		nc = (int8*)Gc__malloc(e__, sizeof(int8) * ns);
		CHECK(error);
		for (i = self->alloced_condition; i > 0;) {
			i--;
			nc[i] = oc[i];
		}
		Gc__free(e__, oc);
		CHECK(error);
		self->condition_stack = nc;
		self->alloced_condition = ns;
	}

	self->condition_stack[self->nb_condition] = self->condition;
	self->nb_condition++;

	self->condition = value;
error:
	return;
}

/**
 * pop the previous "condition" form the stack.
 */
static void pop_condition(Ex **e__, cc__PreIncludeFiles *self)
{
	F("pop_condition");

	if (self->nb_condition == 0) {
		THROW(15, "C compiler internal error.");
		CHECK(error);
	}
	self->nb_condition--;
	self->condition = self->condition_stack[self->nb_condition];
error:
	return;
}

INIT_EXPORT cc__PreIncludeFiles__class cc__PreIncludeFiles__  = {
	// Object
	(utf8*)"cc__PreIncludeFiles",
	sizeof(cc__PreIncludeFiles),
	0,
	__init__,
	__ctor__,
	__dtor__,
	hash_code,
	to_string,

	// IncludeFiles
	clear,
	set_current_file,
	get_current_file_full_path,
	revert_file,
	get_include_file,
	get_file_path,
	init_include_var,
	get_line,
	get_include_tree,
	get_previous_def,
	add_include_path,
	open_file,
	include_once,
	push_condition,
	pop_condition
};

