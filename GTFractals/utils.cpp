#ifndef _AMD64_
#define _AMD64_
#endif


#include "utils.h"

void DebugPrint(const char* format, ...) {
    char buffer[512];  // Buffer per il messaggio
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    OutputDebugStringA(buffer);
}