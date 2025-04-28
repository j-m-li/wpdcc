/* "$Id: $"
 *
 *                          12 December MMIX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <io/Folder.h>
#include <pal/pal.h>

static void __init__(Ex **e__)
{
	io__Folder__.super = &Object__;
}

static void __ctor__(Ex **e__, io__Folder *self)
{
	(void)e__;
	self->handle = (void*)-1;
	self->full_path_name = new_(String);
}

static void __dtor__(Ex **e__, io__Folder *self)
{
	F("__dtor__");
	if (self->handle != (void*)-1) {
		self->_->close(e__, self);
		CHECK(error1);
error1:
		;
	}
	delete_(self->full_path_name);
	CHECK(error2);
error2:
	return;
}

static uint32 hash_code(Ex **e__, io__Folder *self)
{
	F("hash_code");
	uint32 ret;
	ret = self->full_path_name->_->hash_code(e__, self->full_path_name);
	CHECK(error);
	return ret;
error:
	return 0;
} 

static void to_string(Ex **e__, io__Folder *self, String *out)
{
	F("to_string");

	self->full_path_name->_->to_string(e__,
		self->full_path_name, out);
	CHECK(error);
error:
	return;
}



static void _open(Ex **e__, io__Folder *self, String *name, String *mode)
{
	F("_open");
/*	uint32 desired_access;
	uint32 share_mode;
	uint32 creation_disposition;
	uint32 flags_and_attributes = 0;
	uint16 *wname;
	utf8 *tmp1;
	uint16 *tmp;
	int32 nchar;
	int32 nbyte;
	String write;
	String exist;
	String share;
	String temp;

	STR(write, "write");
	STR(exist, "exist");
	STR(share, "share");
	STR(temp, "temp");
*/
// FIXME full path + creation
	if (self->handle != (void*)-1) {
		self->_->close(e__, self);
		CHECK(error1);
	}
	self->handle = win32_find_first_file(name->cstr, &self->next_file);
	return;
error1:
	return;
}

static void _close(Ex **e__, io__Folder *self)
{
	F("_close");
	int32 rc, cnt = 0;

	if (self->handle == (void*)-1) return;

	do {
		rc = win32_find_close(self->handle);
	} while (rc == 0 && cnt++ < 3 && (Sleep(100), 1));
	if (rc == 0) {
		THROW(8, "Cannot close folder.");
		CHECK(error);
	} else {
		self->handle = (void*)-1;
	}
	return;
error:
	return;
}

static int8 find_next_file(Ex **e__, io__Folder *self, String *out)
{
	F("find_next_file");
	String o;

	(void)e__;	
	if (self->next_file == 0) return 0;

	STR(o, self->next_file);
	out->_->add(e__, out, &o);
	CHECK(error);
	win32_find_next_file(self->handle, &self->next_file);
	return 1;
error:
	return 0;
}

INIT_EXPORT io__Folder__class io__Folder__  = {
	// Object
	(utf8*)"io__Folder",
	sizeof(io__Folder),
	0,
	__init__,
	__ctor__,
	__dtor__,
	hash_code,
	to_string,

	// Folder
	_open,
	_close,
	find_next_file
};
