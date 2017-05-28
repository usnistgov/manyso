
// See also http://stackoverflow.com/questions/5919996/how-to-detect-reliably-mac-os-x-ios-linux-windows-in-c-preprocessor
// Copied verbatim from PlatformDetermination.h in order to have a single-include header
#if _WIN64
#  define __ISWINDOWS__
#elif _WIN32
#  define __ISWINDOWS__
#elif __APPLE__
#  define __ISAPPLE__
#elif __linux || __unix || __posix
#  define __ISLINUX__
#elif __powerpc__
#  define __ISPOWERPC__
#else
# pragma error
#endif

#  ifndef EXPORT_CODE
#    if defined(__ISWINDOWS__)
#      define EXPORT_CODE extern "C" __declspec(dllexport)
#    else
#      define EXPORT_CODE extern "C"
#    endif
#  endif
#  ifndef CONVENTION
#    if defined(__ISWINDOWS__)
#      define CONVENTION __stdcall
#    else
#      define CONVENTION
#    endif
#  endif

EXPORT_CODE double CONVENTION add(const double v1, const double v2){
    return v1+v2;
}

EXPORT_CODE void CONVENTION add_inplace(double &v1, const double v2){
    v1 += v2;
}
