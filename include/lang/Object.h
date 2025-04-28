/* "$Id: $"
 *
 *                          07 May MMVIII PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#ifndef LANG__OBJECT_H
#define LANG__OBJECT_H

#define INIT_EXPORT
#define EXE_EXPORT extern
#if !defined(_LIB) && defined(_DYN) && defined(_MSC_VER)
#define EXPORT  __declspec(dllimport)
#else
#define EXPORT extern
#endif



#define new_(c) (c*) new___(e__, (Object__class *)&(c##__))
#define delete_(o) delete___(e__, (Object*)(o))
#define instance_of(t, c) instance_of__((Object*)t, (Object__class *)&(c##__))

#define ASSIGN_FUNC(source, name, dest) { \
	uint8 *s = (uint8*)&source##__; \
	uint8 *d = (uint8*)&dest##__; \
	int32 i = sizeof(void*(*)(void *)); \
	s += ((uint32)&(((source##__class*)0)->name)); \
	d += ((uint32)&(((dest##__class*)0)->name)); \
	while (i > 0) { \
		i--; \
		*d = *s; \
		d++; \
		s++; \
	} \
}


#ifdef _MSC_VER
#define F(n) utf8 *__func__ = (utf8*)n
#else
#define F(n) 
#endif

typedef unsigned char utf8;
typedef signed char int8;
typedef unsigned char uint8;
typedef signed short int16;
typedef unsigned short uint16;
typedef signed int int32;
typedef unsigned int uint32;
#ifdef _MSC_VER
typedef signed __int64 int64;
typedef unsigned __int64 uint64;
#else
typedef signed long long int64;
typedef unsigned long long uint64;
#endif
typedef float float32;
typedef double float64;
typedef long double float80;

struct Exception;
struct String;
struct Serializer;
struct Interface;
struct io__ISerializable;
struct io__Serializer;
struct IComparable;

typedef struct Object {
	struct Object__class *_;
} Object;

typedef struct Object__class {
	utf8 *__name__;
	uint32 __size__;
	struct Object__class *super;
	void (*__init__)(struct Exception **e__);
	void (*__ctor__)(struct Exception **e__, Object *self);
	void (*__dtor__)(struct Exception **e__, Object *self);
	uint32 (*hash_code)(struct Exception **e__, Object *self); 
	void (*to_string)(struct Exception **e__,
		Object *self, struct String *out);
} Object__class; 

EXPORT Object *null;

EXPORT Object__class Object__;

EXPORT Object *new___(struct Exception **e__, Object__class *clas);

EXPORT void delete___(struct Exception **e__, Object *self);

EXPORT int8 instance_of__(Object* t, Object__class *c);

#include <lang/Exception.h>
#include <lang/String.h>
#include <lang/Interface.h>
#include <io/ISerializable.h>
#include <io/Serializer.h>
#include <lang/IComparable.h>
#include <io/File.h>
#include <lang/Gc.h>
#include <gfx/Display.h>

void wpdcc_main(Ex **e__);

#if !defined(_LIB) && defined(_MSC_VER)
void system(char*);
int __stdcall AllocConsole(void);

#define wpdcc_main \
	*wpdcc_dummy = 0; \
	int32 wpdcc_call_main(void) { \
		Ex *e = 0; \
		Ex **e__ = &e; \
		Object *o; \
		o = new_(Object); \
		delete_(o); \
		wpdcc_main(e__); \
		if (*e__ != 0) { \
			Ex *e = *e__; \
			*e__ = 0; \
			echo(e->stack); \
			*e__ = e; \
			return -1; \
		} \
		return 0; \
	} \
	int __stdcall WinMain(void *hInstance, void *hPrevInstance, \
                             unsigned short *lpCmdLine, int nCmdShow) \
	{ \
		gfx__Display__h_instance = hInstance; \
		AllocConsole(); \
		return wpdcc_call_main(); \
		hPrevInstance; lpCmdLine; nCmdShow; \
	} \
	int main(int argc, char argv[]) { \
		return wpdcc_call_main(); \
		argc; argv; \
	} \
	void wpdcc_main
#endif

#endif // LANG__OBJECT_H
