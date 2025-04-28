/* "$Id: $"
 *
 *                          18 May MMVIII PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <lang/String.h>
#include <stdarg.h>

static IComparable__class IComparable__;
static io__ISerializable__class io__ISerializable__;

static void __init__(Ex **e__)
{
	(void)e__;
	String__.super = &Object__;
	INIT_INTERFACE(IComparable, String);
	INIT_INTERFACE(io__ISerializable, String);
	return;
}

static void __ctor__(Ex **e__, String *self)
{
	(void)e__;
	self->size = 0;
	self->cstr = 0;
	return;
}

static void __dtor__(Ex **e__, String *self)
{
	F("__dtor__");

	if (self->cstr) Gc__free(e__, self->cstr);
	CHECK(error1);
error1:
	return;
}

static void dispose_auto(Ex **e__, String *self)
{
	F("dispose_auto");
	
	(void)self;
	THROW(1, "Automatic allocated object.");
	CHECK(error1);
error1:
	return;
}



static uint32 hash_code(Ex **e__, String *self)
{
	F("hash_code");
	utf8 *ptr;
	uint32 val = 0x0;

	ptr = self->_->get_cstr(e__, self);
	CHECK(error);

	while (*ptr != 0) {
		val += *ptr;
		val ^= val << 0x4 | val >> 28;
		ptr++;
	}
	return val;
error:
	return 0;
}

static void to_string(Ex **e__, String *self, String *out)
{
	F("to_string");

	out->_->add(e__, out, self);
	CHECK(error);
error:
	return;
}


static void io__i_serializable(Ex **e__, String *self, io__ISerializable *out)
{
	(void)e__;
	out->_ = &io__ISerializable__;
	out->__self__ = (Object*)self;	
}

static void i_comparable(Ex **e__, String *self, IComparable *out)
{
	(void)e__;
	out->_ = &IComparable__;
	out->__self__ = (Object*)self;	
}

extern void String__add(Ex **e__, String *self, String *in)
{
	F("String__add");
	int i, j, is, ts, ns;
	utf8 *ic, *tc, *nc;

	is = in->_->get_size(e__, in);
	CHECK(error1);
	if (is < 1) return;
	ic = in->_->get_cstr(e__, in);
	CHECK(error1);

	ts = self->size;
	tc = self->cstr;
	ns = is + ts;
	nc = (utf8*)Gc__malloc(e__, ns + 1);
	CHECK(error2);

	for (i = 0; i < ts; i++) {
		nc[i] = tc[i];
	}
	for (i = 0, j = ts; i < is; i++, j++) {
		nc[j] = ic[i];
	}
	nc[ns] = 0;

	if (self->cstr) Gc__free(e__, self->cstr);
	CHECK(error2);

	self->size = ns;
	self->cstr = nc;
	return;
error2:
	Gc__free(e__, nc);
error1:
	return;
}

static void add_auto(Ex **e__, String *self, String *in)
{
	F("add_auto");

	(void)self;
	(void)in;
	THROW(1, "Automatic allocated object.");
	CHECK(error1);
error1:
	return;
}

static void set(Ex **e__, String *self, String *in)
{
	F("set");
	self->_->clear(e__, self);
	CHECK(error);
	self->_->add(e__, self, in);	
	CHECK(error);
error:
	return;
}

static void set_auto(Ex **e__, String *self, String *in)
{
	F("set_auto");
	
	(void)self;
	(void)in;
	THROW(1, "Automatic allocated object.");
	CHECK(error1);
error1:
	return;
}

static void clear(Ex **e__, String *self)
{
	F("clear");
	self->size = 0;
	if (self->cstr) Gc__free(e__, self->cstr);
	CHECK(error);
	self->cstr = 0;
error:
	return;	
}

static void clear_auto(Ex **e__, String *self)
{
	F("clear_auto");

	(void)self;
	THROW(1, "Automatic allocated object.");
	CHECK(error1);
error1:
	return;
}

static utf8 *get_cstr(Ex **e__, String *self)
{
	(void)e__;
	if (!self->cstr) return (utf8*)"";
	return self->cstr;
}

static int32 get_size(Ex **e__, String *self)
{
	(void)e__;
	return self->size;
}

static int32 get_size_auto(Ex **e__, String *self)
{
	utf8 *p = self->cstr;

	(void)e__;
	while (*p) {
		p++;
	}
	return (int32)(p - self->cstr);

}
 
static void grow(Ex **e__, utf8 **out, utf8 **optr, int32 *mout)
{
	F("grow");
	int32 i = (int32)(*optr - *out);
	int32 ns = *mout * 2;
	utf8 *no = (utf8*)Gc__malloc(e__, ns + 1);
	utf8 *o = *out;

	CHECK(error1);

	while (i > 0) {
		i--;
		no[i] = o[i];
	}

	*optr = no + (*optr - *out);
	*mout = ns;
	Gc__free(e__, *out);
	CHECK(error2);
	*out = no;
	return;
error2:
	Gc__free(e__, no);
error1:
	return;
}

static void single_printf(Ex **e__, String *self, utf8 **out, 
	utf8 **optr, int *mout, utf8 **ptr, va_list *ap)
{
	F("single_printf");
	utf8 *p = (*ptr) + 1;
	utf8 *s;
	Object *ob;
	int32 len, i, sign;
	uint32 j, k;
	
	while (*p && *p != '%') {
		switch (*p) {
		case 'c': //int ( single byte)
		case 'C': //int (unicode -> UTF-8)
			return;
		case 'd': //int
			i = va_arg(*ap, int);
			len = 1;
			if (i < 0) {
				sign = -1;
				len++;
				j = ((uint32)i ^ 0xFFFFFFFF) + 1;
			} else {
				sign = 0;
				j = (uint32)i;	
			}
			k = j;
			while (k > 9) {
				k /= 10;
				len++;
			}
			while (*optr - *out + len >= *mout) {
				grow(e__, out, optr, mout);
				CHECK(error1);
			}
			if (sign == -1) {
				(*optr)[0] = '-';
				sign = 1;
			}
			for (i = len - 1; i >= sign; i--) {
				(*optr)[i] = (j % 10) + '0';
				j /= 10;
			}
			
			*optr += len;
			*ptr = p + 1;
			return;
		case 'i': //int
		case 'o': //int
		case 'u': //int
		case 'x': //int
		case 'X': //int
		case 'e': //double
		case 'E': //double
		case 'f': //double
		case 'g': //double
		case 'G': //double
		case 'a': //double
		case 'A': //double
		case 'n': //pointer to int
		case 'p': // void *
			return;;

		case 's': // string 
			s = va_arg(*ap, utf8 *);
			if (s == 0) s = (utf8*)"<NULL>";
			len = 0;
			while (s[len] != 0) len++;

			while (*optr - *out + len >= *mout) {
				grow(e__, out, optr, mout);
				CHECK(error1);
			}
			for (i = 0; i < len; i++) {
				(*optr)[i] = s[i];
			} 
			*optr += len;
			*ptr = p + 1;
			return;
		case 'S': // string
			return;

		case '-': //left align
		case '+': // prefix with sign
		case '0': // prefix with 0
		case ' ': //prefix with blank
		case '#': //prefix with 0x

		case '.':

		case 'h':
		case 'l':
		case 'I':
		case 'w':
			break;
		case 'Z':
			if (*optr != *out) {
				String tmp;
				**optr = 0;
				STR(tmp, *out);
				self->_->add(e__, self, &tmp);
				CHECK(error1);
				*optr = *out;
			}
			ob = va_arg(*ap, Object *);
			if (ob != 0) {
				ob->_->to_string(e__, ob, self);
				CHECK(error1);
			}
			*ptr = p + 1;
			return;		
		} 
		p++;
	}
	*ptr = p;
	**optr = *p;
	(*optr)++;
	return;

error1:
	return;
}

void _printf(Ex **e__, String *self, String *format, ...)
{
	F("_printf");
	utf8 *ptr, *optr;
	utf8 *out;
	int mout = 1023;
	va_list ap;

	va_start(ap, format);

	out = (utf8*)Gc__malloc(e__, mout + 1);
	CHECK(error1);
	optr = out;

	ptr = format->_->get_cstr(e__, format);
	CHECK(error2);

	while (*ptr) {
		if (optr - out >= mout) {
			grow(e__, &out, &optr, &mout);
			CHECK(error2);
		}
		if (*ptr == '%') {
			single_printf(e__, self, &out, &optr, &mout, &ptr, &ap);
			CHECK(error2);
		} else {
			*optr = *ptr;
			optr++;
			ptr++;
		}
	}
	*optr = 0;
	{
		String tmp;

		STR(tmp, out);
		self->_->add(e__, self, &tmp);
		CHECK(error2);
		Gc__free(e__, out);
		CHECK(error2);
	}
	va_end(ap);
	return;
error2:
	Gc__free(e__, out);
error1:
	va_end(ap);
	return;
}

void _printf_auto(Ex **e__, String *self, String *format, ...)
{
	F("_printf_auto");

	(void)self;
	(void)format;
	THROW(1, "Automatic allocated object.");
	CHECK(error1);
error1:
	return;
}

int32 index_of(Ex **e__, String *self, String *needle)
{
	F("index_of");
	int32 i;
	int32 ns = needle->_->get_size(e__, needle);
	utf8 *nt = needle->_->get_cstr(e__, needle);
	int32 size = self->_->get_size(e__, self);
	utf8 *cstr = self->_->get_cstr(e__, self);

	CHECK(error);

	for (i = 0; i < size; i++) {
		int j, k;
		for (j = 0, k = i; j < ns && k < size && nt[j] == cstr[k];
			j++, k++) 
		{
		}
		if (j == ns) return i;
	}
	return -1;
error:
	return 0;
}

static void serialize(Ex **e__, io__ISerializable *itf, io__Serializer *out)
{
	F("serialize");
	String *self = (String*)itf->__self__;
	
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
	String *self = (String*)itf->__self__;

	(void)self;
	(void)in;
	THROW(6, "Method is not implemented.");
	CHECK(error);
error:
	return;
}

static int8 compare_to(Ex **e__, IComparable *itf, Object *in)
{
	F("compare_to");
	String *self = (String*)itf->__self__;

	if (instance_of(in, String) != 0) {
		int32 r;
		String *sin = (String*)in;
		utf8 *t = self->_->get_cstr(e__, self);
		utf8 *i = sin->_->get_cstr(e__, sin);
		
		CHECK(error);

		if (i == 0) {
			if (t == 0) return 0;
			return 1;
		} else if (t == 0) {
			return -1;
		}	
		while (*t && *i && *i == *t) {
			i++;
			t++;
		}
		r = (int32)((uint8)(*t)) - (uint8)(*i);
		return (int8)r;
	}
	return 1;

error:
	return 0;
}

int32 String__utf2ucs(const utf8 *buf, int32 len, int32 *ucs)
{
      if (buf[0] & 0x80) {
        if (buf[0] & 0x40) {
          if (buf[0] & 0x20) {
            if (buf[0] & 0x10) {
              if (buf[0] & 0x08) {
                if (buf[0] & 0x04) {
                  if (buf[0] & 0x02) {
                        /* bad UTF-8 string */
			if (buf[0] & 0x01) {
			} else {
			}
                  } else {
                        /* 0x04000000 - 0x7FFFFFFF */
                  }
                } else if (len > 4) {
                  /* 0x00200000 - 0x03FFFFFF */
                  *ucs =  ((buf[0] & ~0xF8) << 24) +
                          ((buf[1] & ~0x80) << 18) +
                          ((buf[2] & ~0x80) << 12) +
                          ((buf[3] & ~0x80) << 6) +
                           (buf[4] & ~0x80);
                  if (buf[1] & 0x80 && buf[2] & 0x80 && buf[3] & 0x80 &&
                      buf[4] & 0x80 && *ucs < 0x01000000) 
                  {
                    return 5;
                  }
                }
              } else if (len > 3) {
                /* 0x00010000 - 0x001FFFFF */
                *ucs =  ((buf[0] & ~0xF0) << 18) +
                        ((buf[1] & ~0x80) << 12) +
                        ((buf[2] & ~0x80) << 6) +
                         (buf[3] & ~0x80);
                if (buf[1] & 0x80 && buf[2] & 0x80 && buf[3] & 0x80) return 4;
              }
            } else if (len > 2) {
              /* 0x00000800 - 0x0000FFFF */
              *ucs =  ((buf[0] & ~0xE0) << 12) +
                      ((buf[1] & ~0x80) << 6) +
                       (buf[2] & ~0x80);
              if (buf[1] & 0x80 && buf[2] & 0x80) return 3;
            }
          } else if (len > 1) {
            /* 0x00000080 - 0x000007FF */
            *ucs = ((buf[0] & ~0xC0) << 6) +
                    (buf[1] & ~0x80);
            if (buf[1] & 0x80) return 2;
          }
        }
      } else if (len > 0) {
        /* 0x00000000 - 0x0000007F */
        *ucs = buf[0];
        return 1;
      }

      *ucs = -1; /* bad utf-8 string */
      return -1;
}

int32 String__nonspacing(int32 ucs)
{
	return 0;
}

INIT_EXPORT String__class String__  = {
	// Object
	(utf8*)"String",
	sizeof(String) - (sizeof(int32) + sizeof(utf8*)), // substract file/line
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

	String__add,
	set,
	clear,
	get_cstr,
	get_size,
	_printf,
	index_of
};

INIT_EXPORT String__class String_auto__  = {
	// Object
	(utf8*)"String",
	sizeof(String),
	&Object__,
	__init__,
	__ctor__,
	dispose_auto,
	hash_code,
	to_string,

	// String
	io__i_serializable,
	serialize,
	unserialize,
	i_comparable,
	compare_to,
	add_auto,
	set_auto,
	clear_auto,
	get_cstr,
	get_size_auto,
	_printf_auto,
	index_of
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
