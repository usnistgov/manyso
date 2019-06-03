#ifndef MANYSO_ABC_H
#define MANYSO_ABC_H

#include <string>
#include <iostream>
#include <tuple>
#include <map>

#include "manyso/exceptions.h"

class AbstractSharedLibraryWrapper {
private:
    std::map<std::string, void*> pointers;
public:
    enum load_method { SYSTEM_DEFAULT = 0, FROM_FILE, LOAD_LIBRARY, LOAD_LIBRARY_PRISTINE };
protected:
    void* handle = NULL;
    load_method m_load_method;
    bool m_islocked = false;
    virtual void load_from_file(const std::string &path) = 0;
    virtual void load_library(const std::string &path) = 0;
    virtual void load_library_pristine(const std::string &path) = 0;
    virtual void free_library() = 0;
    virtual void* _get_method_pointer(const std::string &method_name) = 0;
public:
    virtual ~AbstractSharedLibraryWrapper() {};
    void load(const std::string &path, load_method method) {
        if (m_islocked) {
            throw InvalidLoad("Class has already been intialized, cannot reinitialize", 0);
        }
        switch (method) {
        case FROM_FILE:
            load_from_file(path); return;
        case LOAD_LIBRARY:
            load_library(path); return;
        case LOAD_LIBRARY_PRISTINE:
            load_library_pristine(path); return;
        default:
            throw InvalidLoad("Can't load", 0);
        }
    };
    void unload() {
        // std::cout << "freeing library" << std::endl;
        free_library();
    };
    void lock(load_method method) {
        if (handle == NULL) {
            throw InvalidLoad("handle is NULL pointer, load was not successful; cannot lock", 0);
        }
        m_load_method = method; m_islocked = true;
    };
    std::tuple<bool, load_method> is_locked() {
        return std::make_tuple(m_islocked, m_load_method);
    };
    template < class retT>
    retT getAddress(const std::string &method_name) {
        void * ptr = _get_method_pointer(method_name);
        if (ptr == NULL) {
            throw InvalidLoad("Unable to load the method: " + method_name, 0);
        }
        else{
            return (retT)(ptr);
        };
    }
    /**
    In parallel execution, multiple calls to dlsym, particularly when multiple threads are all
    trying to call dlsym, can result in threads needing to wait.  dlsym appearrs to be
    single-threaded, so that's not good.  But what we can do is to cache the pointers, so
    that dlsym only gets called as needed, and a map lookup is used (rather than a call
    to dlsym) in order to resolve the pointer.  Massive speedup in code
    */
    void* get_method_pointer(const std::string &method_name) 
    {
        if (pointers.find(method_name) == pointers.end()){
            auto itr = pointers.end();
            bool good_insert;
            std::tie(itr, good_insert) = pointers.emplace(
                std::piecewise_construct,
                std::forward_as_tuple(method_name),
                std::forward_as_tuple(_get_method_pointer(method_name))
            );
            return itr->second;
        }
        else{
            return pointers[method_name];
        }
    };
};

#endif