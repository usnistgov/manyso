#include "manyso/native.h"
#include "manyso/exceptions.h"

#include <string>
#include <iostream>
#include <chrono>

#define LIST_OF_METHOD_NAMES \
    X(add) \
    X(add_inplace)

#define DOUBLE_REF double &

#define add_ARGS double, double
#define add_RETURNS double
#define add_inplace_ARGS DOUBLE_REF, double
#define add_inplace_RETURNS void

// Use X macro magic to define typedefs for each function to be exposed
#define X(name) typedef name ## _RETURNS (*name ## _TYPE)(name ## _ARGS);
    LIST_OF_METHOD_NAMES
#undef X

class Shim{
private:
    WindowsSharedLibraryWrapper wrap;
public:
    Shim(const std::string &path, AbstractSharedLibraryWrapper::load_method method) : wrap(WindowsSharedLibraryWrapper(path, method)) {  };
    
    // Everything totally hardcoded, no macros
    double add_superhard(double v1, double v2) {
        typedef double(*addNumberProc)(double, double);
        return wrap.getAddress<addNumberProc>("add")(v1,v2);
    }
    
    // A couple of macros here, but still mostly hardcoded
    add_RETURNS add_hard(double v1, double v2) {
        return wrap.getAddress<add_TYPE>("add")(v1,v2);
    }
    
    // Now, magic of C++11 starts to appear
    template<class ...Args>
    add_RETURNS add_varargs(Args... args) {
        return wrap.getAddress<add_TYPE>("add")(args...);
    }

    // And now, totally magical, the use of variadic function arguments in concert with type macros
    // See https://stackoverflow.com/a/9103132/1360263 for information about passing the reference for add_inplace
    #define X(name) template<class ...Args> name ## _RETURNS name(Args&&... args){ return wrap.getAddress<name ## _TYPE>(#name)(std::forward<Args>(args)...); };
        LIST_OF_METHOD_NAMES
    #undef X
};

int main(){
    for (auto load_method: {AbstractSharedLibraryWrapper::FROM_FILE, 
                            AbstractSharedLibraryWrapper::LOAD_LIBRARY})
    {
        Shim shim("Debug/so.dll", load_method);
        std::cout << shim.add_superhard(2.0, 3.0) << std::endl; 
        std::cout << shim.add_hard(2.0, 3.0) << std::endl;
        std::cout << shim.add_varargs(2.0, 3.0) << std::endl;
        std::cout << shim.add(2.0, 3.0) << std::endl;

        double v1 = 3.0, v2 = 3;
        shim.add_inplace(v1, v2);
        std::cout << v1 << std::endl;
    }
    return EXIT_SUCCESS;
}