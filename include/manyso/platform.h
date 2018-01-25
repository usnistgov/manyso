#ifndef MANYSO_PLATFORM_H
#define MANYSO_PLATFORM_H

// See also http://stackoverflow.com/questions/5919996/how-to-detect-reliably-mac-os-x-ios-linux-windows-in-c-preprocessor
#if _WIN64
#  define __MANYSOISWINDOWS__
#elif _WIN32
#  define __MANYSOISWINDOWS__
#elif __APPLE__
#  define __ISAPPLE__
#elif __linux || __unix || __posix
#  define __ISLINUX__
#else
# pragma error
#endif

#endif
