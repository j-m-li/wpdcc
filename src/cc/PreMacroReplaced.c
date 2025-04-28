/* "$Id: $"
 *
 *                          27 July MMVIII PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <cc/PreMacroReplaced.h>

static void __init__(Ex **e__)
{
	(void)e__;
	cc__PreMacroReplaced__.super = &Object__;
	return;
}


static void __ctor__(Ex **e__, cc__PreMacroReplaced *self)
{
	F("__ctor__");

	self->replaced = 0;
	self->ma_replaced = 0;
	self->nb_replaced = 0;
	CHECK(error);
	return;
error:
	return;
}

static void __dtor__(Ex **e__, cc__PreMacroReplaced *self)
{
	F("__dtor__");
	int32 i;
	cc__PreMacroReplaced_data *rep;
	
	i = self->nb_replaced;
	rep = self->replaced;
	while (i > 0) {
		i--;
		Gc__free(e__, rep[i].parents_index);
		Gc__free(e__, rep[i].macro_name);
	}
	CHECK(error2);
error2:
	Gc__free(e__, self->replaced);
	CHECK(error1);
error1:
	return;
}

static uint32 hash_code(Ex **e__, cc__PreMacroReplaced *self)
{
	(void)e__;
#ifdef _MSC_VER
	return *((uint32*)&self) >> 4;
#else 
	return (uint32)self >> 4;
#endif
} 

static void to_string(Ex **e__, cc__PreMacroReplaced *self, String *out)
{
	F("to_string");
	String s;

	STR(s, self->_->__name__);
	out->_->add(e__, out, &s);
	CHECK(error);
error:
	return;
}

static int32 push(Ex **e__, cc__PreMacroReplaced *self, utf8 *id, int32 parent)
{
	F("push");
	cc__PreMacroReplaced_data *rep;
	cc__PreMacroReplaced_data *oa;
	int32 l = 0;

	if (self->ma_replaced <= self->nb_replaced) {
		cc__PreMacroReplaced_data *na;
		int32 i;
		int32 ns;

		ns = self->ma_replaced + 1024;
		oa = self->replaced;
		na = Gc__malloc(e__, ns * sizeof(*na));
		CHECK(error1);
	
		i = self->nb_replaced;
		while (i > 0) {
			i--;
			na[i] = oa[i];
		} 
		self->replaced = na;
		self->ma_replaced = ns;
		Gc__free(e__, oa); 
		CHECK(error2);		
	}
	rep = &(self->replaced[self->nb_replaced]);
	self->nb_replaced++;
	rep->parents_index = Gc__malloc(e__, 8 * sizeof(int32));
	CHECK(error1);
	rep->parents_index[0] = parent;
	rep->nb_parents = 1;
	rep->macro_name = id;

	while (id[l] != 0) l++;
	rep->macro_name = Gc__malloc(e__, l + 1);
	l = 0;
	while (id[l] != 0) {
		rep->macro_name[l] = id[l];
		l++;
	}
	rep->macro_name[l] = 0;

	return self->nb_replaced - 1;
error2:
	Gc__free(e__, oa);
error1:
	return 0;
}

static int8 check_is_replaced_added(Ex **e__, cc__PreMacroReplaced *self,
	cc__PreMacroReplaced_data *rep, int32 parent)
{
	F("check_is_replaced_added");
	int32 i;
	int32 *pi;

	(void)__func__;

	i = rep->nb_parents;
	pi = rep->parents_index;

	while (i > 0) {
		i--;
		if (pi[i] == parent) return 1;
	}

	i = rep->nb_parents;
	while (i > 0) {
		int32 idx;

		i--;
		idx = pi[i];
		if (idx > 0 && check_is_replaced_added(e__, self, 
			&(self->replaced[idx]), parent) != 0)
		{
			return 1;
		}
	}
	
	return 0;
}

static void add(Ex **e__, cc__PreMacroReplaced *self, int32 index, 
	int32 parent)
{
	F("add");
	int32 *oa;
	int32 i;
	cc__PreMacroReplaced_data *rep;
	cc__PreMacroReplaced_data *prep;

	if (index < 0 || parent < 0 || index == parent) return;

	
	prep = &(self->replaced[parent]);
	rep = &(self->replaced[index]);
	oa = rep->parents_index;

	if (check_is_replaced_added(e__, self, rep, parent) != 0 ||
		check_is_replaced_added(e__, self, prep, index) != 0) 
	{
		return;
	}

	if ((rep->nb_parents & 0x07) == 0x07) { 
		int32 *na;
		int32 ns;

		ns = ((rep->nb_parents >> 3) + 2) << 3;
		na = Gc__malloc(e__, ns * sizeof(*na));
		CHECK(error1);
	
		i = rep->nb_parents;
		while (i > 0) {
			i--;
			na[i] = oa[i];
		} 
		rep->parents_index = na;
		Gc__free(e__, oa); 
		CHECK(error2);
	}
	rep->parents_index[rep->nb_parents] = parent;
	rep->nb_parents++;
	return;
error2:
	Gc__free(e__, oa);
error1:
	return;
}


/**
 * recursive function to check if 'id' has already been replaced before.
 */
static int8 is_rep(cc__PreMacroReplaced *self, utf8 *id, 
	cc__PreMacroReplaced_data *rep)
{
	F("is_rep");
	int32 i = 0;

	(void)__func__;
	
	while (rep->macro_name[i] == id[i] && id[i] != 0) i++;
	if (rep->macro_name[i] == id[i]) {
		return 1;
	}
	i = rep->nb_parents;
	while (i > 0) {
		int32 idx;

		i--;
		idx = rep->parents_index[i];
		if (idx >= 0 && is_rep(self, id, &self->replaced[idx])
			!= 0)
		{
			return 1;
		}
	}
	return 0;
}

static int8 is_already_replaced(Ex **e__, cc__PreMacroReplaced *self,
	cc__PreToken *tk)
{
	F("is_already_replaced");

	if (tk->replace_index < 0) return 0;

	if (tk->replace_index >= self->nb_replaced) {
		THROW(15, "C compiler internal error.");
		CC__WARN("", tk->file, tk->line, tk->char_pos);
		CHECK(error);
	}

	return is_rep(self, tk->identifier, 
		&self->replaced[tk->replace_index]);
error:
	return 0;
}



static void clean(Ex **e__, cc__PreMacroReplaced *self)
{
	F("clean");
	cc__PreMacroReplaced_data *rep;
	int32 i;

	i = self->nb_replaced;
	rep = self->replaced;
	while (i > 0) {
		i--;
		Gc__free(e__, rep[i].parents_index);
		Gc__free(e__, rep[i].macro_name);
	}
	self->nb_replaced = 0;
	CHECK(error);
	return;
error:
	return;
}

INIT_EXPORT cc__PreMacroReplaced__class cc__PreMacroReplaced__  = {
	// Object
	(utf8*)"cc__PreMacroReplaced",
	sizeof(cc__PreMacroReplaced),
	0,
	__init__,
	__ctor__,
	__dtor__,
	hash_code,
	to_string,

	// PreMacroReplaced
	push,
	add,
	is_already_replaced,
	clean
};

