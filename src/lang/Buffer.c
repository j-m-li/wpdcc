/* "$Id: $"
 *
 *                          21 July MMIX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <lang/Object.h>
#include <lang/Buffer.h>

static IComparable__class IComparable__;
static io__ISerializable__class io__ISerializable__;

static void __init__(Ex **e__)
{
	(void)e__;
	Buffer__.super = &String__;
	ASSIGN_FUNC(String, get_cstr, Buffer);
	ASSIGN_FUNC(String, get_size, Buffer);
	ASSIGN_FUNC(String, printf, Buffer);
	INIT_INTERFACE(IComparable, Buffer);
	INIT_INTERFACE(io__ISerializable, Buffer);
	return;
}

static void __ctor__(Ex **e__, Buffer *self)
{
	(void)e__;
	self->alloced = 0;
	return;
}

static void __dtor__(Ex **e__, Buffer *self)
{
	F("__dtor__");

	(void)__func__;
	(void)e__;
	(void)self;
	return;
}

static uint32 hash_code(Ex **e__, Buffer *self)
{
	F("hash_code");
	uint32 ret;
	ret = self->_->super->hash_code(e__, (String*)self);
	CHECK(error);
	return ret;
error:
	return 0;
} 

static void to_string(Ex **e__, Buffer *self, String *out)
{
	F("to_string");
	String s;

	(void)self;	
	STR(s, "<< Binary Buffer >>");
	out->_->add(e__, out, &s);
	CHECK(error);
error:
	return;
}

static void io__i_serializable(Ex **e__, Buffer *self, io__ISerializable *out)
{
	(void)e__;
	out->_ = &io__ISerializable__;
	out->__self__ = (Object*)self;
}

static void serialize(Ex **e__, io__ISerializable *itf, io__Serializer *out)
{
	F("serialize");
	Buffer *self = (Buffer*)itf->__self__;
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
	Buffer *self = (Buffer*)itf->__self__;
	(void)self;
	(void)in;
	THROW(6, "Method is not implemented.");
	CHECK(error);
error:
	return;
}

static void i_comparable(Ex **e__, Buffer *self, IComparable *out)
{
	(void)e__;
	out->_ = &IComparable__;
	out->__self__ = (Object*)self;	
}

static int8 compare_to(Ex **e__, IComparable *itf, Object *in)
{
	F("compare_to");
	Buffer *self = (Buffer*)itf->__self__;
	int8 ret;	

	if (instance_of(in, Buffer) != 0) {
		Buffer *inb = (Buffer*)in;
		utf8 *cstr = self->cstr;
		utf8 *cstri = inb->cstr;
		int32 size = self->size;
		int32 sizei = inb->size;
		utf8 *i = cstr + size;

		if (size > sizei) {
			return 1;
		} else if (size < sizei) {
			return -1;
		}
		while (cstr < i) {
			if (*cstr != *cstri) {
				if (*cstr > *cstri) {
					return 1;
				}
				return -1;
			}
			cstr++;
			cstri++; 
		}
		return 0;
	}
	ret = self->_->super->compare_to(e__, itf, in);
	CHECK(error);
	return ret;
error:
	return 0;
}


EXPORT void Buffer__add(Ex **e__, Buffer *self, String *in)
{
	F("Buffer__add");
	int32 len;
	utf8 *ptr;
		
	len = in->_->get_size(e__, in);
	CHECK(error);
	ptr = in->_->get_cstr(e__, in);
	CHECK(error);
	if (len <= 0) return;
	self->_->add_data(e__, self, ptr, len);
	return;
error:
	return;

}

static void set(Ex **e__, Buffer *self, String *in)
{
	F("set");

	self->_->clear(e__, self);
	CHECK(error);
	self->_->add(e__, self, in);
	CHECK(error);
error:
	return;	
}

static void clear(Ex **e__, Buffer *self)
{
	F("clear");
	self->size = 0;
	return;
#ifdef _MSC_VER
	__func__;
	e__;
#endif
}

static int32 index_of(Ex **e__, Buffer *self, String *needle)
{
	F("index_of");

	(void)self;
	(void)needle;
	THROW(6, "Method is not implemented.");
	CHECK(error);
error:
	return 0;
}

EXPORT void Buffer__add_data(Ex **e__, Buffer *self, utf8 *data, int32 size)
{
	F("Buffer__add_data");
	utf8 *cstr = self->cstr;
	int32 csize = self->size;
	utf8 *nb;
	int32 i;

	if (csize + size + 1 >= self->alloced) {
		int32 na = csize + size + 512;
		utf8 *ob = self->cstr;

		nb = Gc__malloc(e__, na);
		CHECK(error1);
		for (i = 0; i < csize; i++) {
			nb[i] = ob[i];
		}
		cstr = self->cstr = nb;
		self->alloced = na;
		Gc__free(e__, ob);
		CHECK(error1);
	}

	nb = cstr + csize;
	for (i = 0; i < size; i++) {
		nb[i] = data[i];
	}
	nb[i] = 0;
	self->size = size + csize;
	return;
error1:
	return;
}



INIT_EXPORT Buffer__class Buffer__  = {
	// Object
	(utf8*)"Buffer",
	sizeof(Buffer),
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
	Buffer__add,
	set,
	clear,
	0, // get_cstr,
	0, // get_size,
	0, // _printf,
	index_of,

	// Buffer
	Buffer__add_data
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