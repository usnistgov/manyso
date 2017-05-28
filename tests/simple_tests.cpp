#include "manyso/windows/windows.h"
#include "manyso/exceptions.h"

#include <string>
#include <iostream>
#include <chrono>

typedef double (*addNumberProc)(double, double);

int main(){
    for (auto load_method: {AbstractSharedLibraryWrapper::FROM_FILE, AbstractSharedLibraryWrapper::LOAD_LIBRARY})
    {
        WindowsSharedLibraryWrapper wrap("Release/so.dll", load_method);
        std::cout << wrap.getAddress<addNumberProc>("add")(2, 3) << std::endl;
        long N = 1000000;
        double summer = 0;
        auto startTime = std::chrono::system_clock::now();
        for (auto i = 0; i < N; ++i) {
            summer += wrap.getAddress<addNumberProc>("add")(i, 3);
        }
        auto endTime = std::chrono::system_clock::now();
        std::cout << std::chrono::duration<double>(endTime - startTime).count() << " seconds for one million calls\n";
    }
    return EXIT_SUCCESS;
}