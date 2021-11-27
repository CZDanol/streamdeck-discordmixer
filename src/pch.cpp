#include "pch.h"

void __cdecl dbgprintf(const char *format, ...)
{
	va_list arg;
		va_start(arg, format);

	char msg[1024];
	vsnprintf(msg, sizeof(msg), format, arg);
	OutputDebugStringA(msg);

		va_end(arg);
}
