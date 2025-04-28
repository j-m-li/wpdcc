/* "$Id: $"
 *
 *                          3 May MMIX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#ifndef LANG__PROCESS_H
#define LANG__PROCESS_H

#include <lang/Object.h>

typedef struct Process {
	// Object
	struct Process__class *_;

	// Process
	void *handle;
	void *thread;
} Process;

typedef struct Process__class {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct Object__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, Process *self);
	void (*__dtor__)(Ex **e__, Process *self);
	uint32 (*hash_code)(Ex **e__, Process *self); 
	void (*to_string)(Ex **e__, Process *self, String *out);

	// Process
	void (*create)(Ex **e__, Process *self, String *cmd);
	void (*wait_for)(Ex **e__, Process *self, uint32 millisec);
	void (*close)(Ex **e__, Process *self);
} Process__class; 

EXPORT Process__class Process__;


#endif // LANG__PROCESS_H
