#ifndef INOTIFY_EXCEPTION_H
#define INOTIFY_EXCEPTION_H

#include <exception>

struct MyException : public std::exception
{
    std::string s;
    MyException(std::string ss) : s(ss) {}
    ~MyException() throw () {} // Updated
    const char* what() const throw() { return s.c_str(); }
};

#endif
