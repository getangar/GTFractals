#ifndef UTILS_H
#define UTILS_H

#include "platform.h"
#include <windows.h>
#include <stdio.h>
#include <format>
#include <debugapi.h>
#include <cstdarg>


#include "global.h"

void DebugPrint(const char* format, ...);

#endif