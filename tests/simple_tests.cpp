#include "manyso/abc.h"
#include "manyso/exceptions.h"

#include "MemoryModule.h"

#define WIN32_LEAN_AND_MEAN
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <windows.h>
#include <tchar.h>
#include <string>
#include <iostream>
#include <chrono>

typedef double (*addNumberProc)(double, double);

class WindowsSharedLibraryWrapper : public AbstractSharedLibraryWrapper
{
private:
    /// Read the binary data from the shared library file as a blob of memory
    /// Taken from MemoryModule, under MPL 2.0 license
    void* ReadLibrary(const std::string &fname, size_t &pSize) {
        size_t read;
        void* result;
        FILE* fp;

        auto file_path = TEXT(fname.c_str());

        fp = _tfopen(file_path, _T("rb"));
        if (fp == NULL)
        {
            _tprintf(_T("Can't open shared_library file \"%s\"."), file_path);
            return NULL;
        }

        fseek(fp, 0, SEEK_END);
        pSize = static_cast<size_t>(ftell(fp));
        if (pSize == 0)
        {
            fclose(fp);
            return NULL;
        }

        result = (unsigned char *)malloc(pSize);
        if (result == NULL)
        {
            return NULL;
        }

        fseek(fp, 0, SEEK_SET);
        read = fread(result, 1, pSize, fp);
        fclose(fp);
        if (read != pSize)
        {
            free(result);
            return NULL;
        }

        return result;
    };
protected:
    void load_from_file(const std::string &file_path) override {
        std::size_t psize;
        void *data = ReadLibrary(file_path, psize);
        if (data == NULL){
            return;
        }
        handle = MemoryLoadLibrary(data, psize);
        lock(load_method::FROM_FILE);
    };
    void load_library(const std::string &file_path) override {
        auto cfile_path = TEXT(file_path.c_str());
        handle = LoadLibrary(cfile_path);
        lock(load_method::LOAD_LIBRARY);
    };
    void free_library() override {
        switch (m_load_method) {
        case LOAD_LIBRARY:
            FreeLibrary(static_cast<HMODULE>(handle)); break;
        case FROM_FILE:
            MemoryFreeLibrary(static_cast<HMEMORYMODULE>(handle)); break;
        default:
            throw InvalidUnload("Can't unload library", 0);
        }
    };
    void* get_method_pointer(const std::string &method_name) override {
        switch (m_load_method) {
        case LOAD_LIBRARY:
            return GetProcAddress(static_cast<HMODULE>(handle), method_name.c_str());
        case FROM_FILE:
            return MemoryGetProcAddress(static_cast<HMEMORYMODULE>(handle), method_name.c_str());
        default:
            throw InvalidUnload("Can't unload library", 0);
        }
    };
public:
    WindowsSharedLibraryWrapper(const std::string &path, load_method method = SYSTEM_DEFAULT) {
        load(path, method);
    };
    ~WindowsSharedLibraryWrapper() {
        unload();
    };
};

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