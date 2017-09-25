#ifndef INOTIFY_EXCEPTION_H
#define INOTIFY_EXCEPTION_H


#include <string>
#include <exception>

struct MyException : public std::exception {
    std::string message;

    MyException(std::string input) : message(input) {}

    ~MyException() throw() {}
    const char *what() const throw() { return message.c_str(); }
};

#endif
