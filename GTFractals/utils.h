#ifndef UTILS_H
#define UTILS_H

#include <windows.h>
#include <stdio.h>
#include <format>
#include <debugapi.h>
#include <cstdarg>


#include "global.h"
#include "platform.h"

void DebugPrint(const char* format, ...);

#endif