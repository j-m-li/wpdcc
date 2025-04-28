/* "$Id: $"
 *
 *                          18 May 2008 PUBLIC DOMAIN
 *             The author disclaims copyright to self source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <io/Serial.h>

static void __init__(void)
{
	io__Serial__.super = &io__File__;

	// Object
	WRAP_FUNC(io__Serial, io__File, compare);
	WRAP_FUNC(io__Serial, io__File, hash_code);
	WRAP_FUNC(io__Serial, io__File, to_string);
	WRAP_FUNC(io__Serial, io__File, serialize);
	WRAP_FUNC(io__Serial, io__File, unserialize);

	// String
	WRAP_FUNC(io__Serial, io__File, get_cstr);
	WRAP_FUNC(io__Serial, io__File, get_size);
	WRAP_FUNC(io__Serial, io__File, printf);
	WRAP_FUNC(io__Serial, io__File, index_of);

	// File
	WRAP_FUNC(io__Serial, io__File, open);
	WRAP_FUNC(io__Serial, io__File, close);
	WRAP_FUNC(io__Serial, io__File, seek);
	WRAP_FUNC(io__Serial, io__File, read_line);
	WRAP_FUNC(io__Serial, io__File, end_of_file);
	WRAP_FUNC(io__Serial, io__File, get_line_number);
}

static void __ctor__(io__Serial *self)
{

}

static void dispose(io__Serial *self)
{

}

static void config(io__Serial *self, uint32 baud_rate, uint32 byte_size,
	uint32 parity, uint32 stopnit)
{
	return self->line;
}

EXPORT io__Serial__class io__Serial__  = {
	// Object
	"io__Serial",
	sizeof(io__Serial),
	0,
	__init__,
	__ctor__,
	dispose,
	compare,
	hash_code,
	to_string,
	serialize,
	unserialize,

	// String
	add,
	set,
	clear,
	0, // get_cstr,
	0, // get_size,
	0, // _printf,
	0, //index_of,

	// File
	0, //open,
	0, //close,
	0, //seek,
	0, //read_line,
	0, //end_of_file,
	0, //get_line_number

	// Serial
	config
};

