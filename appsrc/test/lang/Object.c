/* "$Id: $"
 *
 *                          21 June MMVIII PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <lang/Object.h>
#include <io/File.h>
#include <test/Unit.h>

EXE_EXPORT void run___lang__Object(Ex **e__, test__Unit *test)
{
	F("run___lang__Object");
	Object *o;
	String *s;
	String name;
	IComparable ic;

	STR(name, "Object");
	o = new_(Object);
	CHECK(error1);
	s = new_(String);
	CHECK(error2);
	o->_->to_string(e__, o, s);
	CHECK(error2);
	s->_->i_comparable(e__, s, &ic);
	if (ic._->compare_to(e__, &ic, (Object*)&name) != 0) {
		ECHO("clas name error\r\n");
	}
	CHECK(error2);
	
error2:	
	s->_->__dtor__(e__, s);
error1:
	o->_->__dtor__(e__, o);
}
