#include <stdio.h>
#line 100 "hello"
#pragma yo

#include "test1.h"
//#include "test1.c"

#line __LINE__ __FILE__

int main (int argc, char *argv[])
{
	printf("hello %s %d\n", __FILE__, __LINE__, __DATE__, __TIME__,
		__STDC__, __STDC_HOSTED__, __STDC_VERSION__, __STDC_IEC_559__,
		__STDC_MB_MIGHT_NEQ_EC__, __STDC_IEC_559_COMPLEX__,
		__STDC_ISO_10646__);
	return 0;
} */
#line __LINE__ "mix"
