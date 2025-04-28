/* "$Id: $"
 *
 *                          21 June MMVIII PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <lang/Array.h>

static void __init__(Ex **e__)
{
	(void)e__;
	Array__.super = &Object__;
	return;
}

static void __ctor__(Ex **e__, Array *self)
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

static void __dtor__(Ex **e__, Array *self)
{
	F("__dtor__");

	Gc__free(e__, self->data);
	CHECK(error1);
error1:
	return;
}

static uint32 hash_code(Ex **e__, Array *self)
{
	(void)e__;
#ifdef _MSC_VER
	return *((uint32*)&self) >> 4;
#else 
	return (uint32)self >> 4;
#endif
} 

static void to_string(Ex **e__, Array *self, String *out)
{
	F("to_string");
	String s;

	STR(s, self->_->__name__);
	out->_->add(e__, out, &s);
	CHECK(error);
error:
	return;
}

static void grow(Ex **e__, Array *self, uint32 si)
{
	F("grow");
	uint32 i;
	uint32 ns;
	uint32 os;
	void **na;
	void **oa;

	ns = ((si + 65) / 64) * 64;
	os = self->nb_data;
	oa = self->data;
	na = Gc__malloc(e__, sizeof(void*) * ns);
	CHECK(error);

	for (i = 0; i < os; i++) {
		na[i] = oa[i];
	}
	for (i = os; i < ns; i++) {
		na[i] = 0;
	}
	
	self->data = na;
	Gc__free(e__, oa);
	CHECK(error);
	return;
error:
	return;
}

static void set(Ex **e__, Array *self, uint32 index, void *in)
{
	F("set");

	if (index >= self->ma_data) {
		grow(e__, self, index);
		CHECK(error);
	}
	if (index >= self->nb_data) {
		self->nb_data = index + 1;
	}
	self->data[index] = in;	
	return;
error:
	return;
}

static void *get(Ex **e__, Array *self, uint32 index)
{
	F("get");
	
	if (index >= self->nb_data) {
		THROW(11, "Invalid index in Array.");
		CHECK(error);
		return 0;
	}	
	return self->data[index];
error:
	return 0;
}


static uint32 get_length(Ex **e__, Array *self)
{
	F("get_length");

	(void)__func__;
	(void)e__;
	return self->nb_data;
}

static void clear_objects(Ex **e__, Array *self)
{
	F("clear_objects");
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


INIT_EXPORT Array__class Array__  = {
	// Object
	(utf8*)"Array",
	sizeof(Array),
	0,
	__init__,
	__ctor__,
	__dtor__,
	hash_code,
	to_string,

	// Array
	set,
	get,
	get_length,
	clear_objects
};

