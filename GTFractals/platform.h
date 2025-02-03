#ifndef PLATFORM_H
#define PLATFORM_H

// Determina automaticamente l'architettura target
#if defined(_M_X64) || defined(__x86_64__)
#ifndef _AMD64_
#define _AMD64_
#endif
#elif defined(_M_IX86) || defined(__i386__)
#ifndef _X86_
#define _X86_
#endif
#elif defined(_M_ARM) || defined(__arm__)
#ifndef _ARM_
#define _ARM_
#endif
#elif defined(_M_ARM64) || defined(__aarch64__)
#ifndef _ARM64_
#define _ARM64_
#endif
#else
#error "Unsupported architecture. Please update platform.h"
#endif

#endif // PLATFORM_H
