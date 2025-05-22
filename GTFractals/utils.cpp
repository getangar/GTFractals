#include "utils.h"

void DebugPrint(const char* format, ...) {
	char buffer[512];  // Buffer for storing the formatted string
	va_list args;
	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);

	OutputDebugStringA(buffer);
}