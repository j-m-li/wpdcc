#include <stdlib.h>
#include <stdio.h>
#include "xostime.h"

int  ddt2str(char *string, char *format, time_dz *tm)
{
	return sprintf(string, "FIXME");
}

char *strmov(char *dest, const char *src)
{
	dest[0] = 0;
	strcat(dest, src);
	return dest + strlen(dest);
}

char *strnmov(char *dest, const char *src, size_t maxlen)
{

	while (maxlen > 0 && *src) {
		maxlen--;
		*dest = *src;
		dest++;
		src++;
	}
	*dest = 0;
	return dest;

}