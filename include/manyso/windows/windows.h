#ifndef MANYSO_WINDOWS_WINDOWS_H
#define MANYSO_WINDOWS_WINDOWS_H

#include "manyso/abc.h"

#if defined(__MANYSOISWINDOWS__)

#define WIN32_LEAN_AND_MEAN
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <windows.h>
#include <tchar.h>

#include "MemoryModule.h"

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
            throw std::range_error("Unable to determine the size of the file by calling ftell()");
            return NULL;
        }

        result = (unsigned char *)malloc(pSize);
        if (result == NULL)
        {
            throw std::range_error("Unable to allocate a blob of memory of "+std::to_string(static_cast<int>(pSize))+" bytes.");
            return NULL;
        }

        fseek(fp, 0, SEEK_SET);
        read = fread(result, 1, pSize, fp);
        fclose(fp);
        if (read != pSize)
        {
            free(result);
            throw std::range_error("Size of blob read in ["+std::to_string(read)+"] not equal to expected size [" + std::to_string(static_cast<int>(pSize)) + "].");
            return NULL;
        }

        return result;
    };
protected:
    void load_from_file(const std::string &file_path) override {
        std::size_t psize;
        void *data = ReadLibrary(file_path, psize);
        if (data == NULL) {
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

#endif

#endif