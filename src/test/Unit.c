/* "$Id: $"
 *
 *                          07 May MMVIII PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <test/Unit.h>

static void __init__(Ex **e__)
{
	(void)e__;
	test__Unit__.super = &Object__;
	ASSIGN_FUNC(Object, hash_code, test__Unit);
	ASSIGN_FUNC(Object, to_string, test__Unit);
	return;
}

static void __ctor__(Ex **e__, test__Unit *self)
{

	return;
#ifdef _MSC_VER
	e__;
	self;
#endif
}

static void __dtor__(Ex **e__, test__Unit *self)
{
	F("__dtor__");

	(void)__func__;
	(void)e__;
	(void)self;
	return;
}



INIT_EXPORT test__Unit__class test__Unit__  = {
	// Object
	(utf8*)"test__Unit",
	sizeof(test__Unit),
	0,
	__init__,
	__ctor__,
	__dtor__,
	0, // hash_code,
	0, // to_string,
};
