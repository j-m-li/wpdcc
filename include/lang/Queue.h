/* "$Id: $"
 *
 *                          3 May MMIX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#ifndef LANG__QUEUE_H
#define LANG__QUEUE_H

#include <lang/Object.h>

typedef struct Queue {
	// Object
	struct Queue__class *_;

	// Queue
	void **data;
	uint32 nb_data;
	uint32 ma_data;
	uint32 start;
	uint32 end;
} Queue;

typedef struct Queue__class {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct Object__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, Queue *self);
	void (*__dtor__)(Ex **e__, Queue *self);
	uint32 (*hash_code)(Ex **e__, Queue *self); 
	void (*to_string)(Ex **e__, Queue *self, String *out);

	// Queue
	void (*enqueue)(Ex **e__, Queue *self, uint32, void *in);
	void (*dequeue)(Ex **e__, Queue *self, void **out);
	uint32 (*get_length)(Ex **e__, Queue *self);
	void (*clear_objects)(Ex **e__, Queue *self);
} Queue__class; 

EXPORT Queue__class Queue__;


#endif // LANG__QUEUE_H
