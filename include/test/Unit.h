#ifndef TEST__UNIT_H
#define TEST__UNIT_H

#include <lang/Object.h>

#define unit_log(level, text) echo(text)
#define UNIT_LOG(level, text) ECHO(text)
#define UNIT_WARNING(level, text) ECHO(text)
#define UNIT_ERROR(level, text) ECHO(text)

typedef struct test__Unit {
	// Object
	struct test__Unit__class *_;

	// Unit
} test__Unit;

typedef struct test__Unit__class {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct Object__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, test__Unit *self);
	void (*__dtor__)(Ex **e__, test__Unit *self);
	uint32 (*hash_code)(Ex **e__, test__Unit *self); 
	void (*to_string)(Ex **e__, test__Unit *self, 
		struct String *out);

	// Unit
} test__Unit__class; 

EXPORT test__Unit__class test__Unit__;

#endif // TEST__UNIT_H
