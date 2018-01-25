
#ifndef MANYSO_NATIVE_H
#define MANYSO_NATIVE_H

// Depending on the platform, alias to the appopriate class
#include "manyso/platform.h"

#if defined(__MANYSOISWINDOWS__)
#include "manyso/windows/windows.h"
typedef WindowsSharedLibraryWrapper NativeSharedLibraryWrapper;
#else
#include "manyso/linux/linux.h"
typedef LinuxSharedLibraryWrapper NativeSharedLibraryWrapper;
#endif

#endif