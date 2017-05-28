#ifndef MANYSO_EXCEPTIONS
#define MANYSO_EXCEPTIONS

#include <exception>

class ManySOBaseException : public std::exception {
public:
    ManySOBaseException(const std::string &err, std::size_t code) throw() : m_code(code), m_err(err) {};
    virtual const char* what() const throw() override { 
        return m_err.c_str(); 
    };
private:
    std::size_t m_code;
    std::string m_err;
};

class InvalidLoad : public ManySOBaseException {
public:
    InvalidLoad(const std::string &err, std::size_t code) throw() : ManySOBaseException(err, code) {};
};

class InvalidUnload : public ManySOBaseException {
public:
    InvalidUnload(const std::string &err, std::size_t code) throw() : ManySOBaseException(err, code) {};
};

#endif