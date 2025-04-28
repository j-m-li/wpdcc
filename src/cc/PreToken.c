/* "$Id: $"
 *
 *                          24 July MMVIII PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <cc/PreToken.h>

static void __init__(Ex **e__)
{
	(void)e__;

	cc__PreToken__.super = &Object__;
	return;
}

static void __ctor__(Ex **e__, cc__PreToken *self)
{
	(void)e__;
	self->identifier = 0;
	return;
}

static void __dtor__(Ex **e__, cc__PreToken *self)
{
	F("__dtor__");

	if (self->identifier != 0) Gc__free(e__, self->identifier);
	CHECK(error1);
error1:
	return;
}

static uint32 hash_code(Ex **e__, cc__PreToken *self)
{
	F("hash_code");
	utf8 *ptr;
	uint32 val = 0x0;

	ptr = self->identifier;
	if (ptr == 0) return 0;

	while (*ptr != 0) {
		val += *ptr;
		val ^= val << 0x4 | val >> 28;
		ptr++;
	}
	return val;
} 

static void to_string(Ex **e__, cc__PreToken *self, String *out)
{
	F("to_string");
	String s;

	STR(s, self->identifier);
	out->_->add(e__, out, &s);
	CHECK(error);
error:
	return;
}

INIT_EXPORT cc__PreToken__class cc__PreToken__  = {
	// Object
	(utf8*)"cc__PreToken",
	sizeof(cc__PreToken),
	0,
	__init__,
	__ctor__,
	__dtor__,
	hash_code,
	to_string

	// PreToken
};

