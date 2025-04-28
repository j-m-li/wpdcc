/* "$Id: $"
 *
 *                          28 July 2008 PUBLIC DOMAIN
 *             The author disclaims copyright to self source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <io/Serial.h>
#include <test/Unit.h>

EXPORT void run___io__Serial(test__Unit *test)
{
	io__Serial *o;
	S(name, "COM");
	S(mode, "write, exist");

	o = new_(io__Serial);
	
	o->_->open(o, name, mode);
	
	o->_->dispose(o);
}
