/* "$Id: $"
 *
 *                          21 June MMVIII PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <lang/Process.h>

static void __init__(Ex **e__)
{
	(void)e__;
	Process__.super = &Object__;
	return;
}

static void __ctor__(Ex **e__, Process *self)
{
	F("__ctor__");
	self->handle = (void*)-1;
	return;
error:
	return;
}

static void __dtor__(Ex **e__, Process *self)
{
	F("__dtor__");

	self->_->close(e__, self);
	CHECK(error1);
error1:
	return;
}

static uint32 hash_code(Ex **e__, Process *self)
{
	(void)e__;
#ifdef _MSC_VER
	return *((uint32*)&self) >> 4;
#else 
	return (uint32)self >> 4;
#endif
} 

static void to_string(Ex **e__, Process *self, String *out)
{
	F("to_string");
	String s;

	STR(s, self->_->__name__);
	out->_->add(e__, out, &s);
	CHECK(error);
error:
	return;
}

static void create(Ex **e__, Process *self, String *cmd)
{
	win32_create_process(cmd->cstr, &self->handle, &self->thread);
}

static void wait_for(Ex **e__, Process *self, uint32 millisec)
{
	win32_wait_for_process(self->handle, millisec);
}

static void close(Ex **e__, Process *self)
{
	if (self->handle == (void*)-1) return;
	win32_close_process(self->handle, self->thread);
}


INIT_EXPORT Process__class Process__  = {
	// Object
	(utf8*)"Process",
	sizeof(Process),
	0,
	__init__,
	__ctor__,
	__dtor__,
	hash_code,
	to_string,

	// Process
	create,
	wait_for,
	close
};

