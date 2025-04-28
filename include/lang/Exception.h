/* "$Id: $"
 *
 *                          07 May MMVIII PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#ifndef LANG__EXCEPTION_H
#define LANG__EXCEPTION_H

#define Ex struct Exception

#define CHECK(label) {if (*e__ != 0) { \
	(*e__)->_->trace(e__, *e__, (utf8*)__func__, \
		__LINE__, (utf8*)__FILE__); \
	goto label; \
}}

#define LOG(s) {if (*e__ != 0) { \
	(*e__)->_->log(e__, *e__, s); \
}}

#define THROW(e, d) { \
	String tmp; \
	STR(tmp, d); \
	Exception__throw(e__, e, &tmp); \
}

typedef struct Exception {
	// Object
	struct Exception__class *_;

	// Exception
	int32 id;
	struct String *stack;
} Exception;

typedef struct Exception__class {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct Object__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, Exception *self);
	void (*__dtor__)(Ex **e__, Exception *self);
	uint32 (*hash_code)(Ex **e__, Exception *self); 
	void (*to_string)(Ex **e__, Exception *self, 
		struct String *out);

	// Exception
	void (*trace)(Ex **e__, Exception *self, utf8 *func, int line,
		utf8 *file);
	void (*set_id)(Ex **e__, Exception *self, int32 id,
		struct String *desc);
	void (*log)(Ex **e__, Exception *self, struct String *txt);
} Exception__class; 



EXPORT Exception__class Exception__;

EXPORT void Exception__throw(Ex **e__, int32 id, struct String *desc);

/*******************************************************************************
Exception list:
0, "Success."
1, "Automatic allocated object."
2, "Cannot allocate memory."
3, "Cannot allocate managed memory."
4,  "Object is an Interface."
5, "Cannot write to file."
6, "Method is not implemented."
7, "Cannot open file."
8, "Cannot close file."
9, "Cannot seek file."
10, "C compiler fatal error."
11, "Invalid index in Array."
12,
13,
14,
15, "C compiler internal error."

C compiler :
-100000 to -100053 PreProcessor
-100100 to -100101 Env
-100200 to -100201 Parser
(free:  )
*******************************************************************************/ 
#endif // LANG__EXCEPTION_H
