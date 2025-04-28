/* "$Id: $"
 *
 *                          15 June MMVIII PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <lang/Object.h>

static void __init__(Ex **e__)
{
	(void)e__;
	Exception__.super = &Object__;
	return;
}

static void __ctor__(Ex **e__, Exception *self)
{
	self->stack = new_(String);	
}

static void __dtor__(Ex **e__, Exception *self)
{
	F("__dtor__");

	delete_(self->stack);
	CHECK(error1);
error1:
	return;
}

static uint32 hash_code(Ex **e__, Exception *self)
{
	(void)e__;
#ifdef _MSC_VER
	return *((uint32*)&self) >> 4;
#else 
	return (uint32)self >> 4;
#endif
}

static void to_string(Ex **e__, Exception *self, String *out)
{
	F("to_string");

	out->_->add(e__, out, self->stack);
	CHECK(error);
error:
	return;
}

static void trace(Ex **e__, Exception *self, utf8 *func, int line,
	utf8 *file)
{
	F("trace");
	String f;
	String fi;
	String fu;
	Ex *e = 0;

	STR(f, "%Z @ %d: %Z\n");
	STR(fi, file);
	STR(fu, func);

	if (self->id < 0) return;
	
	self->stack->_->printf(&e, self->stack, &f, &fi, line, &fu);
	if (e != 0) {
		delete_(e);
	}
	return;
#ifdef _MSC_VER
	__func__;
#endif
}

static void set_id(Ex **e__, Exception *self, int32 id, String *desc)
{
	F("set_id");
	String f;
	Ex *e = 0;

	(void)__func__;
	STR(f, "E%d: %Z\n");
	self->id = id;
	self->stack->_->printf(&e, self->stack, &f, id, desc);

	if (e != 0) {
		delete_(e);
	}
	return;
}

void log_(Ex **e__, Exception *self, String *txt)
{
	F("log");
	Ex *e = 0;

	(void)__func__;
	self->stack->_->add(&e, self->stack, txt);
	if (e != 0) {
		delete_(e);
	}
	return;
}

INIT_EXPORT Exception__class Exception__  = {
	// Object
	(utf8*)"Exception",
	sizeof(Exception),
	0,
	__init__,
	__ctor__,
	__dtor__,
	hash_code,
	to_string,

	// Exception
	trace,
	set_id,
	log_
};

EXPORT void Exception__throw(Ex **e__, int32 id, String *desc)
{
	Ex *e;

	if (*e__ == 0) {
		e = new_(Exception);
		if (e != 0 && *e__ == 0) {
			e->_->set_id(e__, e, id, desc);
			*e__ = e;
		}
	}
	// e = *e__;
	//e->_->trace(e__, e,  func, line, file);
}
