#ifndef PLATFORM_H
#define PLATFORM_H

// Imposta la versione minima di Windows a Windows 98
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif

// Imposta la versione minima della Windows API a Windows 98
#ifndef WINVER
#define WINVER 0x0400
#endif

// Controllo architettura
#if !defined(_AMD64_) && !defined(_X86_) && !defined(_ARM_) && !defined(_ARM64_)
#if defined(_M_X64) || defined(__x86_64__)
#define _AMD64_
#elif defined(_M_IX86) || defined(__i386__)
#define _X86_
#elif defined(_M_ARM) || defined(__arm__)
#define _ARM_
#elif defined(_M_ARM64) || defined(__aarch64__)
#define _ARM64_
#else
#error "Unsupported architecture. Please update platform.h"
#endif
#endif

#endif // PLATFORM_H
