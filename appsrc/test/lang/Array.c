/* "$Id: $"
 *
 *                          21 June 2008 PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <lang/Array.h>
#include <test/Unit.h>

EXE_EXPORT void run___lang__Array(Ex **e__, test__Unit *test)
{
	F("run___lang__Array");
	String name;
	Array *o;
	Array *o2;
	Array *a;
	String *s, *s2 = 0;
	int32 i;
	Object *d, *r;
	Object *arr[10000];

	String__auto(e__, &name, "Array");
	CHECK(error);
	o = new_(Array);
	CHECK(error);
	o2 = new_(Array);
	CHECK(error);
	s = new_(String);
	CHECK(error);

	o->_->to_string(o, e__, s);
	CHECK(error);
	if (s->_->compare(s, e__, (Object*)&name) != 0) {
		ECHO("class name error\r\n");
		CHECK(error);
	}
	CHECK(error);
	
	o->_->push(o, e__, (Object*)&name);
	CHECK(error);
	o->_->push(o, e__, (Object*)null);
	CHECK(error);
	o->_->pop(o, e__, (Object**)&s2);
	CHECK(error);
	o->_->pop(o, e__, (Object**)&s2);
	CHECK(error);
	if (s2 != &name) {
		UNIT_LOG(0, "push/pop error\r\n");
		CHECK(error);
	}
	o->_->pop(o, e__, (Object**)&s2);

	CHECK(error);
	if (s2 != 0) {
		UNIT_LOG(0, "supplementary pop error\r\n");
		CHECK(error);
	}
	

	for (i = 0; i < 100; i++) {
		d = new_(Object);
		CHECK(error);
		o->_->enqueue(o, e__, (Object*)d);
		CHECK(error);
		r = (Object*)o->_->peek(o, e__);
		CHECK(error);
		if (i == 0 && r != d) {
			UNIT_LOG(0, "peek error 1\r\n");
			CHECK(error);
		} else if (i != 0 && r == d) {
			UNIT_LOG(0, "peek error 2\r\n");
			CHECK(error);
		}
	}

	for (i = 0; i < 100; i++) {
		o->_->dequeue(o, e__, (Object**)&r);
		CHECK(error);
		delete_(r);
	}
	if (r != d) {
		UNIT_LOG(0, "dequeue error\r\n");
		CHECK(error);
	}
	
	//UNIT_LOG(1, r->_->__name__);
	//UNIT_LOG(0, "DEC_REF error\r\n");
	
	for (i = 0; i < 10000; i++) {
		utf8 buf[5];
		int32 *n = (int32*)buf;
		int32 j = 9999 - i;

		*n = 0;
		//j = i;
		buf[4] = 0;
		buf[3] = (0x7F & j) | 0x80;
		buf[2] = (0x7F & (j >> 7)) | 0x80;
		buf[1] = (0x7F & (j >> 14)) | 0x80;
		buf[0] = (0x7F & (j >> 21)) | 0x80;
		s2 = new_(String);
		CHECK(error);
		{
			String t;

			String__auto(e__, &t, buf);
			CHECK(error);
			s2->_->set(s2, e__, &t);
			CHECK(error);
		}
		arr[i] = (Object*)s2;
		o->_->set(o, e__, i, (Object*)s2);
		CHECK(error);
	}

	o->_->remove_range(o, e__, 3, 10);
	CHECK(error);

	for (i = 0; i < 10000; i++) {
		d = o->_->get(o, e__, i);
		CHECK(error);
		if (d != arr[i]) {
			if (i >= 3 && i < 13) {
			} else {
				UNIT_LOG(0, "get error\r\n");
				CHECK(error);
			}
		} else {
			if (i >= 3 && i < 13) {
				UNIT_LOG(0, "get error\r\n");
				CHECK(error);
			} else {
				
			}
		}
	}

	o->_->sort(o, e__);
	CHECK(error);

	for (i = 10000 - 1; i >= 0; i--) {
		uint32 l;
		int32 j = 9999 - i;
		String *q;
		
		o->_->unset(o, e__, i, &d);
		delete_(d);
		CHECK(error);
		q = (String*) d;
		
		if (j < 3) {
			if (d != arr[j]) {
				UNIT_LOG(0, "unset error 1\r\n");
				CHECK(error);
			}
		} else if (j <= 9989) {
			if (d != arr[j + 10]) {
				UNIT_LOG(0, "unset error 2\r\n");
				CHECK(error);
			}
		} else {
			if (d) {
				UNIT_LOG(0, "unset error (remove_range\r\n");
				CHECK(error);
			}
		}

		if (i % 100 == 0) {
			// length is very slow!!!
			l = o->_->get_length(o, e__);
			CHECK(error);
			if (l != (uint32)i) {
				UNIT_LOG(0, "length error\r\n");
				CHECK(error);
			}
		}
	}

	o->_->clear(o, e__);
	CHECK(error);

	for (i = 0; i < 10000; i++) {
		utf8 buf[5];
		int32 *n = (int32*)buf;
		int32 j = 9999 - i;

		*n = 0;
		//j = i;
		buf[4] = 0;
		buf[3] = (0x7F & j) | 0x80;
		buf[2] = (0x7F & (j >> 7)) | 0x80;
		buf[1] = (0x7F & (j >> 14)) | 0x80;
		buf[0] = (0x7F & (j >> 21)) | 0x80;
		s2 = new_(String);
		CHECK(error);
		{
			String t;

			String__auto(e__, &t, buf);
			s2->_->set(s2, e__, &t);
			CHECK(error);
		}
		arr[i] = (Object*)s2;
		if (i < 500) {
			o->_->set(o, e__, i, (Object*)s2);
			CHECK(error);
		} else if (i < 1000) {
			o2->_->set(o2, e__, i - 500, (Object*)s2);
			CHECK(error);
		} else {
			o->_->set(o, e__, i - 500, (Object*)s2);
			CHECK(error);	
		}
	}
	o->_->insert(o, e__, 500, o2);
	CHECK(error);

	a = o->_->get_values(o, e__);
	CHECK(error);
	if (a->_->get_length(a, e__) != 10000) {
		CHECK(error);
		UNIT_LOG(0, "get_values error before!!!\r\n");
		CHECK(error);
	}
	a->_->dispose(a, e__);
	CHECK(error);

	for (i = 0; i < 10000; i++) {
		o->_->unset(o, e__, i, &d);
		CHECK(error);
//		if (d) DEC_REF(d);
		if (d != arr[i]) {
			UNIT_LOG(0, "unset error (insert)\r\n");
			CHECK(error);
		}
	}
	a = o->_->get_keys(o, e__);
	CHECK(error);
	if (a->_->get_length(a, e__) != 0) {
		UNIT_LOG(0, "get_key error before!!!\r\n");
		CHECK(error);
	}
	CHECK(error);
	a->_->dispose(a, e__);
	CHECK(error);

	o->_->clear(o, e__);
	CHECK(error);
	{
		String one, two, three, second;
		Object *o1 = new_(Object);
		Object *o2 = new_(Object);
		Object *o3 = new_(Object);

		String__auto(e__, &one, "hello self is me a long text 1");
		String__auto(e__, &two, "hello self is me a long text 2");
		String__auto(e__, &three, "hello self is me a long text 3");
		String__auto(e__, &second, "hello self is me a long text 2");

		o->_->add(o, e__, (Object*)&one, o1);
		CHECK(error);
		o->_->add(o, e__, (Object*)&two, o2);
		CHECK(error);
		o->_->add(o, e__, (Object*)&three, o3);
		CHECK(error);

		a = o->_->get_keys(o, e__);
		CHECK(error);
		if (a->_->get_length(a, e__) != 3) {
			CHECK(error);
			UNIT_LOG(0, "get_keys error\r\n");
			CHECK(error);
		}
		a->_->dispose(a, e__);
		CHECK(error);

		a = o->_->get_values(o, e__);
		CHECK(error);
		if (a->_->get_length(a, e__) != 3) {
			CHECK(error);
			UNIT_LOG(0, "get_values error\r\n");
			CHECK(error);
		}
		a->_->dispose(a, e__);
		CHECK(error);

		r = o->_->item(o, e__, (Object*)&second);
		CHECK(error);
		if (r != o2) {
			UNIT_LOG(0, "item error\r\n");
			CHECK(error);
		}
		o->_->remove(o, e__, (Object*)&one, &r);
		CHECK(error);
	
		if (r != o1) {
			UNIT_LOG(0, "remove 1 error\r\n");
			CHECK(error);
		}
		o->_->remove(o, e__, (Object*)&two, &r);
		CHECK(error);
		
		if (r != o2) {
			UNIT_LOG(0, "remove 2 error\r\n");
			CHECK(error);
		}
		o->_->remove(o, e__, (Object*)&three, &r);
		CHECK(error);
	
		if (r != o3) {
			UNIT_LOG(0, "remove 3 error\r\n");
			CHECK(error);
		}	
	}

	o->_->clear(o, e__);
	CHECK(error);

	for (i = 0; i < 10000; i++) {
		utf8 buf[5];
		int32 *n = (int32*)buf;

		buf[4] = 0;
		buf[0] = (0x7F & i) + 1;
		buf[1] = (0x7F & (i >> 7)) + 1;
		buf[2] = (0x7F & (i >> 14)) + 1;
		buf[3] = (0x7F & (i >> 21)) + 1;
		s2 = new_(String);
		CHECK(error);
		{
			String t;

			String__auto(e__, &t, buf);
			CHECK(error);
			s2->_->set(s2, e__, &t);
			CHECK(error);
		}
		arr[i] = (Object*)s2;
		o->_->add(o, e__, (Object*)s2, (Object*)s2);
		CHECK(error);
	}

	for (i = 0; i < 10000; i++) {
		o->_->remove(o, e__, arr[i], &r);
		CHECK(error);
		delete_(r);
		if (r != arr[i]) {
			UNIT_LOG(0, "remove hashing error\r\n");
			CHECK(error);
		}		
	}

		
	o->_->dispose(o, e__);
	CHECK(error);
	o2->_->dispose(o2, e__);
	CHECK(error);
	s->_->dispose(s, e__);
	CHECK(error);
error:
	return;
}
