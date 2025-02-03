#include "manyso/native.h"
#include "manyso/exceptions.h"
#include "manyso/platform.h"

#include <string>
#include <iostream>
#include <chrono>
#include <cstring>

#include "REFPROP_lib.h"

class REFPROPWrap{
private:
    NativeSharedLibraryWrapper wrap;
public:
    REFPROPWrap(const std::string &path, AbstractSharedLibraryWrapper::load_method method) : wrap(NativeSharedLibraryWrapper(path, method)) {  };

    // And now, totally magical, the use of variadic function arguments in concert with type macros
    // Add many methods, each corresponding to a 1-to-1 wrapper of a function from the shared library
    #define X(name) template<class ...Args> void name(Args&&... args){ return wrap.getAddress<name ## _POINTER>(#name)(std::forward<Args>(args)...); };
        LIST_OF_REFPROP_FUNCTION_NAMES
    #undef X
};

int main(){
    for (auto load_method: {AbstractSharedLibraryWrapper::LOAD_LIBRARY, AbstractSharedLibraryWrapper::FROM_FILE })
    {
#if defined(__MANYSOISWINDOWS__)
        std::string root = "D:/Code/REFPROP-cmake/build/10/Release/";
#else
        std::string root = std::string(std::getenv("RPPREFIX"));
#endif
        REFPROPWrap wrap(root+"librefprop.so", load_method);
        
        char path[255];
        strcpy(path, root.c_str());
        wrap.SETPATHdll(path, 255); 

        long nc = 1, ierr = 0;
        char herr[255], hfld[10000] = "CO2.FLD", hmx[255] = "HMX.BNC", href[4] = "DEF";
        
        wrap.SETUPdll(nc, hfld, hmx, href, ierr, herr, 10000, 255, 3, 255);
        if (ierr == 0){
            double z[] = { 1.0 }, x[] = { 1.0 }, y[] = { 1.0 }, T = 300, p = 101.325, d = -1, dl = -1, dv = -1, h = -1, s = -1, u = -1, cp = -1, cv = -1, q = -1, w = -1;
            wrap.TPFLSHdll(T, p, z, d, dl, dv, x, y, h, s, u, cp, cv, w, q, ierr, herr, 255);
            std::cout << d << std::endl;
        }
        else{
            std::cout << "Load error: " << herr << std::endl;
        }
    }
    return EXIT_SUCCESS;
}
