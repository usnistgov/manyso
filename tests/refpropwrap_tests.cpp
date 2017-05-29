#include "manyso/windows/windows.h"
#include "manyso/exceptions.h"

#include <string>
#include <iostream>
#include <chrono>

#include "REFPROP_lib.h"

class REFPROPWrap{
private:
    WindowsSharedLibraryWrapper wrap;
public:
    REFPROPWrap(const std::string &path, AbstractSharedLibraryWrapper::load_method method) : wrap(WindowsSharedLibraryWrapper(path, method)) {  };

    // And now, totally magical, the use of variadic function arguments in concert with type macros
    // See https://stackoverflow.com/a/9103132/1360263 for information about passing the reference for add_inplace
    #define X(name) template<class ...Args> void name(Args&&... args){ return wrap.getAddress<name ## _POINTER>(#name)(std::forward<Args>(args)...); };
        LIST_OF_REFPROP_FUNCTION_NAMES
    #undef X
};

int main(){
    for (auto load_method: {AbstractSharedLibraryWrapper::LOAD_LIBRARY})
    {
        REFPROPWrap wrap("REFPRP64.dll", load_method);
        
        //char path[255] = "c:/Program Files (x86)/REFPROP/";
        //wrap.SETPATHdll(path, 255); 

        long nc = 1, ierr = 0;
        char herr[255];
        
        wrap.SETUPdll(nc, "CO2.FLD", "HMX.BNC", "DEF", ierr, herr, 10000, 255, 3, 255);
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