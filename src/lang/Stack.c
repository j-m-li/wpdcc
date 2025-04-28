/* "$Id: $"
 *
 *                          21 June MMVIII PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <lang/Stack.h>

static void __init__(Ex **e__)
{
	Stack__.super = &Object__;
	(void)e__;
	return;
}

static void __ctor__(Ex **e__, Stack *self)
{
	F("__ctor__");

	self->data = Gc__malloc(e__, sizeof(void*) * 32);
	CHECK(error);
	self->ma_data = 32;
	self->nb_data = 0;
	return;
error:
	return;
}

static void __dtor__(Ex **e__, Stack *self)
{
	F("__dtor__");

	Gc__free(e__, self->data);
	CHECK(error1);
error1:
	return;
}

static uint32 hash_code(Ex **e__, Stack *self)
{
	(void)e__;
#ifdef _MSC_VER
	return *((uint32*)&self) >> 4;
#else 
	return (uint32)self >> 4;
#endif
} 

static void to_string(Ex **e__, Stack *self, String *out)
{
	F("to_string");
	String s;

	STR(s, self->_->__name__);
	out->_->add(e__, out, &s);
	CHECK(error);
error:
	return;
}

static void grow(Ex **e__, Stack *self)
{
	F("grow");
	uint32 i;
	uint32 ns;
	uint32 os;
	void **na;
	void **oa;

	ns = self->ma_data + 64;
	os = self->nb_data;
	oa = self->data;
	na = Gc__malloc(e__, sizeof(void*) * ns);
	CHECK(error);

	for (i = 0; i < os; i++) {
		na[i] = oa[i];
	}
	self->data = na;
	self->ma_data = ns;
	Gc__free(e__, oa);
	CHECK(error);
	return;
error:
	return;
}

static void push(Ex **e__, Stack *self, void *in)
{
	F("push");

	if (self->nb_data >= self->ma_data) {
		grow(e__, self);
		CHECK(error);
	}
	self->data[self->nb_data++] = in;	
	return;
error:
	return;
}

static void pop(Ex **e__, Stack *self, void **out)
{
	F("pop");

	if (self->nb_data == 0) {
		*out = 0;
		return;
	}
	*out = self->data[--self->nb_data];	
	return;
#ifdef _MSC_VER
	__func__;
	e__;
#endif
}


static uint32 get_length(Ex **e__, Stack *self)
{
	F("get_length");

	(void)__func__;
	(void)e__;
	return self->nb_data;
}

static void clear_objects(Ex **e__, Stack *self)
{
	F("get_length");
	uint32 i;

	for (i = 0; i < self->nb_data; i++) {
		if (self->data[i] != 0) {
			delete_((Object*)self->data[i]);
		}	
	}
	self->nb_data = 0;
	CHECK(error);
	return;
error:
	return;
}

INIT_EXPORT Stack__class Stack__  = {
	// Object
	(utf8*)"Stack",
	sizeof(Stack),
	0,
	__init__,
	__ctor__,
	__dtor__,
	hash_code,
	to_string,

	// Stack
	push,
	pop,
	get_length,
	clear_objects
};

