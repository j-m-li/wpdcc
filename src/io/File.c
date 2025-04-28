/* "$Id: $"
 *
 *                          18 May MMVIII PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <lang/Object.h>
#include <pal/pal.h>

static IComparable__class IComparable__;
static io__ISerializable__class io__ISerializable__;

static void __init__(Ex **e__)
{
	io__File__.super = &String__;

	ASSIGN_FUNC(String, get_cstr, io__File);
	ASSIGN_FUNC(String, get_size, io__File);
	ASSIGN_FUNC(String, printf, io__File);
	INIT_INTERFACE(IComparable, io__File);
	INIT_INTERFACE(io__ISerializable, io__File);

	io__File__stdin = new_(io__File);
	io__File__stdout = new_(io__File);
	io__File__stderr = new_(io__File);
	io__File__stdin->handle = GetStdHandle(STD_INPUT_HANDLE);
	io__File__stdout->handle = GetStdHandle(STD_OUTPUT_HANDLE); 
	io__File__stderr->handle = GetStdHandle(STD_ERROR_HANDLE);
}

static void __ctor__(Ex **e__, io__File *self)
{
	self->handle = (void*)-1;
	self->full_path_name = new_(String);
	self->alloced = 0;
	self->read_len = 0;
	self->eof = 0;
}

static void __dtor__(Ex **e__, io__File *self)
{
	F("__dtor__");

	if (self == io__File__stderr ||
		self == io__File__stdin ||
		self == io__File__stdout)
	{
		return;
	}

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

static uint32 hash_code(Ex **e__, io__File *self)
{
	F("hash_code");
	uint32 ret;
	ret = self->full_path_name->_->hash_code(e__, self->full_path_name);
	CHECK(error);
	return ret;
error:
	return 0;
} 

static void to_string(Ex **e__, io__File *self, String *out)
{
	F("to_string");

	self->full_path_name->_->to_string(e__,
		self->full_path_name, out);
	CHECK(error);
error:
	return;
}
static void io__i_serializable(Ex **e__, io__File *self, io__ISerializable *out)
{
	(void)e__;
	out->_ = &io__ISerializable__;
	out->__self__ = (Object*)self;
}

static void serialize(Ex **e__, io__ISerializable *itf, io__Serializer *out)
{
	F("serialize");
	io__File *self = (io__File*)itf->__self__;
	(void)self;
	(void)out;
	THROW(6, "Method is not implemented.");
	CHECK(error);
error:
	return;
}

static void unserialize(Ex **e__, io__ISerializable *itf, io__Serializer *in)
{
	F("unserialize");
	io__File *self = (io__File*)itf->__self__;

	(void)self;
	(void)in;
	THROW(6, "Method is not implemented.");
	CHECK(error);
error:
	return;
}


static void i_comparable(Ex **e__, io__File *self, IComparable *out)
{
	(void)e__;
	out->_ = &IComparable__;
	out->__self__ = (Object*)self;
}

static int8 compare_to(Ex **e__, IComparable *itf, Object *in)
{
	F("compare_to");
	io__File *self = (io__File*)itf->__self__;
	int8 ret;

	if (instance_of(in, io__File) != 0) {
		IComparable i;
		self->full_path_name->_->i_comparable(e__,
			self->full_path_name, &i);
		ret = i._->compare_to(e__, &i, 
			(Object*)((io__File*)in)->full_path_name);
		CHECK(error);
		return ret;
	}	
	ret = self->_->super->compare_to(e__, itf, in);
	CHECK(error);
	return ret;
error:
	return 0;
}

static void add(Ex **e__, io__File *self, String *in)
{
	F("add");
	uint32 len;
	uint32 wrote;
	uint32 rc = 0;
	uint32 wrt;
	int8 *ptr;
		
	len = in->_->get_size(e__, in);
	CHECK(error);
	ptr = (int8*)in->_->get_cstr(e__, in);
	CHECK(error);
	if (len <= 0) return;

	wrt = (len > 0x0FFF) ? 0x0FFF : len;
	while (len > 0
		&& (rc = WriteFile(self->handle, ptr, wrt, &wrote, 0)) != 0
		&& wrote > 0)
	{
		len -= wrote;
		wrt = (len > 0x0FFF) ? 0x0FFF : len;
		ptr = ptr + wrote;
	}
	
	if (rc == 0 || len != 0) {
		THROW(5, "Cannot write to file.");
		CHECK(error);
		return;
	}
error:
	return;

}

static void set(Ex **e__, io__File *self, String *in)
{
	F("set");

	self->_->clear(e__, self);
	CHECK(error);
	self->_->add(e__, self, in);
	CHECK(error);
error:
	return;	
}

static void clear(Ex **e__, io__File *self)
{
	F("clear");
	int32 u = 0;

	(void)__func__;
	(void)e__;
	SetFilePointer(self->handle, 0, (long*)&u, FILE_BEGIN);
	SetEndOfFile(self->handle);
	return;
}

static int32 index_of(Ex **e__, io__File *self, String *needle)
{
	F("index_of");

	(void)self;
	(void)needle;
	THROW(6, "Method is not implemented.");
	CHECK(error);
error:
	return 0;
}

static void _open(Ex **e__, io__File *self, String *name, String *mode)
{
	F("_open");
	uint32 desired_access;
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
	self->line = 0;
	self->eof = 0;

	nchar = MultiByteToWideChar(CP_UTF8, 0, 
		(char*)name->_->get_cstr(e__, name),
		name->_->get_size(e__, name), 
		NULL, 0) + 3;
	CHECK(error1);
	wname = (uint16*) Gc__malloc(e__, nchar * sizeof(uint16));
	CHECK(error1);
	nchar = MultiByteToWideChar(CP_UTF8, 0, 
		(char*)name->_->get_cstr(e__, name),
		name->_->get_size(e__, name), 
		wname, nchar);
	CHECK(error2);
	wname[nchar] = 0;

	nbyte = GetFullPathNameW(wname, 0, 0, 0) + 3;
	tmp = (uint16*) Gc__malloc(e__, nbyte * sizeof(uint16));
	CHECK(error2);
	GetFullPathNameW(wname, nbyte, tmp, 0);

	nbyte = WideCharToMultiByte(CP_UTF8, 0, tmp, -1, 0, 0, 0, 0);
	tmp1 = (utf8*) Gc__malloc(e__, nbyte);
	CHECK(error3);
	nbyte = WideCharToMultiByte(CP_UTF8, 0, tmp, -1, (char*)tmp1, nbyte, 0, 0);
	{
		String tmps;

		STR(tmps, tmp1);
		self->full_path_name->_->set(e__,
			self->full_path_name, &tmps);
		CHECK(error4);
	}

	if (mode->_->index_of(e__, mode, &write) >= 0) {
		CHECK(error4);
		desired_access = GENERIC_READ | GENERIC_WRITE;
	} else {
		desired_access = GENERIC_READ;
	}
	if (mode->_->index_of(e__, mode, &exist) >= 0) {
		CHECK(error4);
		creation_disposition = OPEN_EXISTING;
	} else {
		creation_disposition = OPEN_ALWAYS;
	}
	if (mode->_->index_of(e__, mode, &share) >= 0) {
		CHECK(error4);
		share_mode = FILE_SHARE_READ | FILE_SHARE_WRITE;
	} else {
		share_mode = 0;
	}
	if (mode->_->index_of(e__, mode, &temp) >= 0) {
		CHECK(error4);
		flags_and_attributes = FILE_ATTRIBUTE_TEMPORARY
                               | FILE_ATTRIBUTE_HIDDEN
                               | FILE_FLAG_DELETE_ON_CLOSE;
	} else {
		flags_and_attributes = FILE_ATTRIBUTE_NORMAL;
	}
	flags_and_attributes |= FILE_FLAG_RANDOM_ACCESS;

	self->handle = CreateFileW(wname,
			desired_access,
			share_mode,
			0,
			creation_disposition,
			flags_and_attributes,
			0);
	
	if (self->handle == INVALID_HANDLE_VALUE ){
		String nl;

		STR(nl, "\n");
		THROW(7, "Cannot open file.");
		LOG(name);
		LOG(&nl);
		CHECK(error4);
	}
error4:
	Gc__free(e__, tmp);
error3:
	Gc__free(e__, tmp1);
error2:
	Gc__free(e__, wname);
error1:
	return;
}

static void _close(Ex **e__, io__File *self)
{
	F("close");
	int32 rc, cnt = 0;

	if (self->handle == (void*)-1) return;

	do {
		rc = CloseHandle(self->handle);
	} while (rc == 0 && cnt++ < 3 && (Sleep(100), 1));
	if (rc == 0) {
		THROW(8, "Cannot close file.");
		CHECK(error);
	} else {
		self->handle = (void*)-1;
	}
	return;
error:
	return;
}

static void _seek(Ex **e__, io__File *self, int64 pos)
{
	F("_seek");
	int32 upper_bits = (long int)((pos>>32) & 0x7fffffff);
	int32 lower_bits = (long int)(pos & 0xffffffff);
	uint32 rc;

	self->line = 0;
	rc = SetFilePointer(self->handle, lower_bits, (long*)
		&upper_bits, FILE_BEGIN);
	if (rc == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR) {
		THROW(9, "Cannot seek file.");
		CHECK(error);
	}
	return;
error:
	return;
}

static void read_buf(Ex **e__, utf8 **cstr, int *alloced, int *read_len,
	void *handle, int8 *eof)
{
	F("read_buf");
	uint32 got, rc;
	utf8 *nb;
	utf8 *ob = *cstr;
	int32 s;
	int32 i;

	if (*eof != 0) return;
	if (*alloced != *read_len) {
		rc = ReadFile(handle, *cstr + *read_len, 
			*alloced - *read_len, &got, 0);
		if (rc && got == 0) {
			*eof = 1;
		}
		*read_len += got;
		return;
	}

	s = *alloced;
	nb = (utf8*) Gc__malloc(e__, s + 1024);
	CHECK(error1);

	for (i = 0; i < s; i++) {
		nb[i] = ob[i];
	}
	Gc__free(e__, ob);
	CHECK(error2);

	*cstr = nb;
	*alloced = s + 1023;
	read_buf(e__, cstr, alloced, read_len, handle, eof);
	return;
error2:
	Gc__free(e__, nb);
error1:
	return;
}

static void read_line(Ex **e__, io__File *self, String *out)
{
	F("read_line");
	int32 i = 0;

	if (self->read_len < 1 && !self->eof) {
		read_buf(e__, &self->cstr, &self->alloced, 
			&self->read_len, self->handle, &self->eof);
		CHECK(error);
	}
	while (i < self->read_len) {
		if (self->cstr[i] == '\n' || self->cstr[i] == '\r') {
			int32 j;
			if (i + 1 == self->read_len) {
				read_buf(e__, &self->cstr, &self->alloced, 
					&self->read_len, self->handle,
					&self->eof);
				CHECK(error);
			}
			i++;
			if (i < self->read_len &&
				(self->cstr[i] == '\r' ||
				self->cstr[i] == '\n')) 
			{
				i++;
			}
			self->size = i;
			out->_->add(e__, out, (String*)self);
			CHECK(error);
			self->size = 0;
			self->line++;
			for (j = 0; i < self->read_len; i++, j++) {
				self->cstr[j] = self->cstr[i];
			} 
			self->cstr[j] = 0;
			self->read_len = j;
			return;
		}
		i++;
		if (i == self->read_len) {
			read_buf(e__, &self->cstr, &self->alloced, 
				&self->read_len, self->handle, &self->eof);
			CHECK(error);
		}
	}
	if (self->read_len > 0) {
		self->size = self->read_len;
		out->_->add(e__, out, (String*)self);
		CHECK(error);
		self->size = 0;
		self->read_len = 0;
		self->line++;
	}
error:
	return;
}

static int8 end_of_file(Ex **e__, io__File *self)
{
	(void)e__;
	if (self->eof && self->read_len < 1) return 1;
	return 0;
}

static int32 get_line_number(Ex **e__, io__File *self)
{
	(void)e__;
	return self->line;
}

INIT_EXPORT io__File__class io__File__  = {
	// Object
	(utf8*)"io__File",
	sizeof(io__File),
	0,
	__init__,
	__ctor__,
	__dtor__,
	hash_code,
	to_string,

	// String
	io__i_serializable,
	serialize,
	unserialize,

	i_comparable,
	compare_to,

	add,
	set,
	clear,
	0, // get_cstr,
	0, // get_size,
	0, // _printf,
	index_of,

	// File
	_open,
	_close,
	_seek,
	read_line,
	end_of_file,
	get_line_number
};

static io__ISerializable__class io__ISerializable__  = {
	// Object
	0, 0, 0, 0, 0, 0, 0, 0,

	// io__ISerializable
	serialize,
	unserialize
};

static IComparable__class IComparable__  = {
	// Object
	0, 0, 0, 0, 0, 0, 0, 0,

	// IComparable
	compare_to
};

INIT_EXPORT io__File *io__File__stdin = 0;
INIT_EXPORT io__File *io__File__stdout = 0;
INIT_EXPORT io__File *io__File__stderr = 0;
