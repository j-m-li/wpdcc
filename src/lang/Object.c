/* "$Id: $"
 *
 *                          15 June MMVIII PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <pal/pal.h>
#include <lang/Object.h>
#include <io/File.h>

static void __init__(Ex **e__)
{
	(void)e__;
	Object__.super = 0;
	return;
}

static void __ctor__(Ex **e__, Object *self)
{
	(void)e__;
	(void)self;	
	return;
}

static void __dtor__(Ex **e__, Object *self)
{
	(void)e__;
	(void)self;	
}


static uint32 hash_code(Ex **e__, Object *self)
{
	(void)e__;	
#ifdef _MSC_VER
	return *((uint32*)&self) >> 4;
#else 
	return (uint32)self >> 4;
#endif
}


static void to_string(Ex **e__, Object *self, String *out)
{
	F("to_string");
	String s;

	STR(s, self->_->__name__);
	out->_->add(e__, out, &s);
	CHECK(error);
error:
	return;
}


INIT_EXPORT Object__class Object__  = {
	(utf8*)"Object",
	sizeof(Object),
	0,
	__init__,
	__ctor__,
	__dtor__,
	hash_code,
	to_string
};

INIT_EXPORT Object *null = 0;

static void dispose__(Ex **e__, Object__class *clas, Object *self)
{
	F("dispose__");

	(void)__func__;
	clas->__dtor__(e__, self);
	/* don't care about errors */	
	if (clas->super) {
		dispose__(e__, clas->super, self);
	}
	return;
}

static void construct__(Ex **e__, Object__class *clas, Object *self)
{
	F("construct__");
		
	if (clas) {
		construct__(e__, clas->super, self);
		CHECK(error);
		clas->__ctor__(e__, self);
		CHECK(error1);
	}
	return;
error1:
	dispose__(e__, clas, self);
error:
	return;
}

static void construct_init__(Ex **e__, Object__class *clas, Object *self)
{
	F("construct_init_");
	if (clas) {
		if (clas->super == 0) {
			clas->__init__(e__);
			CHECK(error);
			construct_init__(e__, clas->super, self);
			CHECK(error);
		} else {
			construct__(e__, clas->super, self);
			CHECK(error);
		}
		clas->__ctor__(e__, self);
		CHECK(error1);
	}
	return;
error1:
	dispose__(e__, clas, self);
error:
	return;
}

EXPORT void delete___(Ex **e__, Object *self)
{
	F("delete__");
	Ex *e = *e__;

	if (self == 0) return;
	
	*e__ = 0;

	dispose__(e__, self->_, self);
	CHECK(error);
error:
	Gc__free(e__, self);

	if (e) {
		Ex *ee;

		ee = *e__;
		*e__ = e;
		if (ee != 0) delete_(ee);
	}

	return;
}

EXPORT Object *new___(Ex **e__, Object__class *clas)
{
	F("new__");
	Object *self;

	if (*e__ != 0) return 0;

	self = (Object*) Gc__malloc(e__, clas->__size__);
	CHECK(error);
	
	self->_ = clas;
	if (clas->super == 0) {
		construct_init__(e__, clas, self);
		CHECK(error);
		// initialize stdin stdout stderr
		if (io__File__.super == 0) {
			io__File__.__init__(e__);
			CHECK(error);
			null = (Object*)new_(String);
			CHECK(error);
			Gc__.__init__(e__);
			CHECK(error);
		}
	} else {
		self->_ = clas;
		construct__(e__, clas, self);
		CHECK(error);
	}
	return self;

error:
	Gc__free(e__, self);
	return 0;
}

EXPORT int8 instance_of__(Object* t, Object__class *c)
{
	Object__class *tc = t->_;
	
	do {
		if (tc == c) return 1;
		tc = tc->super;
	} while (tc);
	return 0;
}
