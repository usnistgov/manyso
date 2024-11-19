#ifndef MANYSO_LINUX_LINUX_H
#define MANYSO_LINUX_LINUX_H

#include "manyso/abc.h"

#if !defined(__MANYSOISWINDOWS__)

#include <dlfcn.h>
#if !defined(__APPLE__)
#include <link.h>
#endif

class LinuxSharedLibraryWrapper : public AbstractSharedLibraryWrapper
{
protected:
    virtual void load_from_file(const std::string &path) override{ 
        throw InvalidLoad("Loading from file not supported on linux", 0); 
    };
    void load_library(const std::string &file_path) override {
        handle = dlopen(file_path.c_str(), RTLD_NOW);
        if (handle == nullptr){
            std::string err(dlerror());
            throw InvalidLoad("Could not load library from the path:"+file_path+" with error: "+err, 0);
        }
        lock(load_method::LOAD_LIBRARY);
    };
#if !defined(__APPLE__)
    void load_library_pristine(const std::string &file_path) override {
  
        handle = dlmopen(LM_ID_NEWLM, file_path.c_str(), RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND);
        if (handle == nullptr){
            std::string err(dlerror());
            throw InvalidLoad("Could not load library from the path:"+file_path+" with error: "+err, 0);
        }
        lock(load_method::LOAD_LIBRARY_PRISTINE);
    };
#else
    void load_library_pristine(const std::string &file_path) override {
        throw InvalidLoad("Could not load library from the path:"+file_path+" on APPLE arch", 0);
    };
#endif
    void free_library() override {
	// Both methods free in the same way
        int retcode = dlclose (handle);
        if (retcode != 0){
            std::string err(dlerror());
            throw InvalidUnload("Could not unload library with error: "+err, 0);
        }
        handle = nullptr;
    };
    void* _get_method_pointer(const std::string &method_name) override {
        return dlsym(handle, method_name.c_str());
    };
public:
    LinuxSharedLibraryWrapper(const std::string &path, load_method method = SYSTEM_DEFAULT) {
        load(path, method);
    };
    virtual ~LinuxSharedLibraryWrapper() {
        unload();
    };
};

#endif

#endif
