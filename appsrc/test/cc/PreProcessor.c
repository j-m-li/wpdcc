/* "$Id: $"
 *
 *                          21 June MMVIII PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <cc/PreProcessor.h>
#include <test/Unit.h>

static int32 process_file(Ex **e__, test__Unit *test, String *fi, int32 n)
{
	F("process_file");
	cc__PreProcessor *o;
	cc__Env *env;

	o = new_(cc__PreProcessor);
	CHECK(error1);

	env = new_(cc__Env);
	CHECK(error3);
	env->_->init(e__, env);
	CHECK(error4);
	o->_->set_env(e__, o, env);
	CHECK(error4);
	env->preprocessor = o;
	o->_->process_file(e__, o, fi);
	CHECK(error4);
	o->_->run(e__, o);
	
	if (*e__) {
		Ex *e = *e__;	
		
		*e__ = 0;	
		echo(e->stack);
		if (e->id != n) {
			UNIT_LOG(0, "wrong last error!!!\r\n");
			THROW(10, "C compiler fatal error.");
			CHECK(error4);
		}
		delete_(e);
	} else {
		UNIT_LOG(0, "error not thrown\r\n");
		THROW(10, "C compiler fatal error.");
		CHECK(error4);
	}

	
	delete_(env);
	delete_(o);
	
	return 1;
error4:
	delete_(env);
error3:
	delete_(o);
error1:
	return 0;
}

static void check_errors(Ex **e__, test__Unit *test)
{
	F("check_errors");
	String fi;
	utf8 f[] = "cc/PreProcessor/100001.c";
	int32 n = -100001;

	STR(fi, f);

	while (process_file(e__, test, &fi, n) != 0) {
		int32 i = 22;
		do {
			i--;
			if (f[i] == '9') {
				f[i] = '0';
			} else {
				f[i]++;
			}
		} while  (f[i] == '0');
		n--;
	}
	CHECK(error99);
	return;
error99:
	return; 
}

EXE_EXPORT void run___cc__PreProcessor(Ex **e__, test__Unit *test)
{
	F("run___cc__PreProcessor");
	cc__PreProcessor *o;
	String *s;
	String name;
	String fi;
	cc__Env *env;
	IComparable ic;

	STR(name, "cc__PreProcessor");
	STR(fi, "cc/PreProcessor/sanity_check.c");
	o = new_(cc__PreProcessor);
	CHECK(error1);
	s = new_(String);
	CHECK(error2);
	o->_->to_string(e__, o, s);
	CHECK(error3);
	s->_->i_comparable(e__, s, &ic);
	if (ic._->compare_to(e__, &ic, (Object*)&name) != 0) {
		UNIT_LOG(0, "class name error\r\n");
		CHECK(error3);
	}
	CHECK(error3);

	env = new_(cc__Env);
	CHECK(error3);
	env->_->init(e__, env);
	CHECK(error4);
	s->_->set(e__, s, &fi);
	env->sources->_->set(e__, env->sources, 0, s);
	//env->_->c_preprocess(e__, env, &fi);
	CHECK(error4);
	env->_->run(e__, env);
	CHECK(error4);

	delete_(env);

	check_errors(e__, test);
	
	//delete_(s);
	delete_(o);
	
	return;
error4:
	delete_(env);
error3:
	//delete_(s);
error2:
	delete_(o);
error1:
	return;
}
