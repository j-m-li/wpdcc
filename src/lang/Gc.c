/* "$Id: $"
 *
 *                          15 June MMVIII PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <lang/Gc.h>
#include <windows.h>

static Exception *cannot_allocate_managed_memory = (void*)1;
static Exception *cannot_allocate_memory = (void*)1;

static void __init__(Ex **e__)
{
	String mm; 
	String ma; 
	
	STR(mm, "Cannot allocate managed memory.");
	STR(ma, "Cannot allocate memory.");
	Gc__.super = &Object__;

	cannot_allocate_managed_memory = new_(Exception);
	cannot_allocate_managed_memory->id = 3;
	cannot_allocate_managed_memory->stack = &mm;

	cannot_allocate_memory = new_(Exception);
	cannot_allocate_memory->id = 2;
	cannot_allocate_memory->stack = &ma;
	
}

static void __ctor__(Ex **e__, Gc *self)
{
	(void)e__;
	(void)self;	
	return;
}

static void __dtor__(Ex **e__, Gc *self)
{
	F("__dtor__");
	(void)__func__;
	(void)e__;
	(void)self;
	return;
}

static uint32 hash_code(Ex **e__, Gc *self)
{
	(void)e__;
#ifdef _MSC_VER
	return *((uint32*)&self) >> 4;
#else 
	return (uint32)self >> 4;
#endif
}

static void to_string(Ex **e__, Gc *self, String *out)
{
	F("to_string");
	String s;

	STR(s, self->_->__name__);
	out->_->add(e__, out, &s);
	CHECK(error);
error:
	return;
}

INIT_EXPORT Gc__class Gc__  = {
	(utf8*)"Gc",
	sizeof(Gc),
	0,
	__init__,
	__ctor__,
	__dtor__,
	hash_code,
	to_string
};


EXPORT void *Gc__malloc(Ex **e__, int size)
{
	F("Gc__malloc");
	void *ret;

	(void)__func__;
	ret = HeapAlloc(GetProcessHeap(), 0, size);

	if (ret == 0 && size != 0) {
		*e__ = cannot_allocate_memory;
		return 0;
	}
	return ret;
//error:
//	return 0;
}

EXPORT void Gc__free(Ex **e__, void *data)
{
	(void)e__;	

	if (data == cannot_allocate_memory) return;
	if (((int32)data & 0xFFFF) == 0x70c8) {
		data = data;
	}
	HeapFree(GetProcessHeap(), 0, data);
	return;
}

EXPORT void *Gc__new(Ex **e__, int size)
{
	F("Gc__new");
	void *ret;
	(void)__func__;
	ret = HeapAlloc(GetProcessHeap(), 0, size);

	if (ret == 0 && size != 0) {
		*e__ = cannot_allocate_managed_memory;
		return 0;
	}
	return ret;
//error:
//	return 0;
}

EXPORT void Gc__delete(Ex **e__, void *data)
{
	(void)e__;

	if (((int32)data & 0xFFFF) == 0x70c8) {
		data = data;
	}

	if (data == cannot_allocate_managed_memory) return;
	HeapFree(GetProcessHeap(), 0, data);
	return;
}

