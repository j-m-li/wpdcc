/* "$Id: $"
 *
 *                          21 June MMVIII PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <test/Unit.h>
#include <pal/win32.h>

int main(int argc, char **argv)
{
	F("main");
	int i;
	String hello;
	test__Unit *test;
	Ex *e = 0;
	Ex **e__ = &e;
	Ex *ee;
	utf8 *tests[] = {
	//	"run___lang__Object",
	//	"run___lang__Array",
		//"run___io__Serial",
		(utf8*)"run___cc__PreProcessor"
	};

	STR(hello, "Starting Unit test...\r\n");
	test = new_(test__Unit);
	CHECK(error);

	unit_log(0, &hello);
	CHECK(error);

	for (i = 0; i < sizeof(tests) / sizeof(utf8*); i++)
	{
		void (*func)(Ex**, test__Unit*) = (void (*)(Ex**, test__Unit*))
			GetProcAddress(GetModuleHandleA(0), (char*)tests[i]);

		if (func) {
			UNIT_LOG(0, tests[i]);
			CHECK(error);
			UNIT_LOG(0, "() ");
			CHECK(error);
			func(e__, test);
			CHECK(exception);
			goto end_exception;
exception:
			ee = e;
			e = 0;			
			unit_log(0, ee->stack);
			UNIT_LOG(0, "\r\nEXCEPTION!!!\r\n");
			delete_(ee);
			CHECK(error);
end_exception:
			UNIT_LOG(0, "\r\n");
			CHECK(error);
		} else {
			UNIT_LOG(0, "\r\nError!!! GetProc failed\r\n");
			CHECK(error);
		}
	}	
	
	test->_->__dtor__(e__, test);
	CHECK(error);
	UNIT_LOG(0, "End of Unit test!\r\n");
	system("pause");
error:
	return 0;
}
