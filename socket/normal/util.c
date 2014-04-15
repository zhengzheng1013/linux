#include "util.h"

int _printf(const char *format, ...)
{
	va_list list;
	int result;

	printf("in process[%d]: ", getpid());
	
	va_start(list, format);
	result = vprintf(format, list);
	va_end(list);

	return result;
}
