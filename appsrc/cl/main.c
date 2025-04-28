/* "$Id: $"
 *
 *                          15 Jully MMIX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 *
 * 
 */

 
#include <cc/Env.h>

/*
Usage: cc [-options] filename

-h <text> Shows information regarding <text>
-g Full symbolic debug
-O[0-4] Optimization level (Default:2)
-c Disable linking
-S Do not assemble
-E Run only the preprocessor
-C Preprocessor does not discard comments
-P Preprocessor does not generate #line directives
-Dmacro[=def] Define a macro
-Umacro Undefine a macro
-H Print the name of all header file used
-o file Write output in file
-v Print commands executed at every stage of compilation
-w Inhibit all warning messages
-Wall Enables all warning messages
-p Generate profiling extra code
-a Generate profiling extra code for basic block
-Q Print out each function name
-llibrary Link against the library name library
-Idir Add dir to the head of the list of headers directories
-Ldir Add dir to the list of library directories

*/

void wpdcc_main(Ex **e__)
{
	F("wpdcc_main");
	cc__Env *env;

	env = new_(cc__Env);
	CHECK(error1);

	ECHO("The PUBLIC DOMAIN C compiler MMVI-MMXI.\n");

	env->_->init(e__, env);
	CHECK(error2);
	env->_->run(e__, env);
	CHECK(error2);

	delete_(env);
	CHECK(error2);
	return;
error2:
	delete_(env);
error1:
	return;
}
