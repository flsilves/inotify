#ifndef BACKLOG_H
#define BACKLOG_H

#include <iostream>

#include <thread>


#include <cstdlib>

#include <dirent.h>
#include <cstdarg>
#include <cassert>
#include <cstring>
#include <set>

#include "mutex.h"
#include <sys/inotify.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include "exception.h"
using namespace std;

class ConcurrentBacklog {

private:
    Mutex pMutex;
    pthread_cond_t backlogNotEmpty;
    std::set<string> backlog;

public:
    ConcurrentBacklog();
    ~ConcurrentBacklog();
    void print(ostream &os = std::cout) const;
    int size() const;
    string pop();
    size_t erase(string value);
    void push(string value);
};

void operator<<(ostream &os, ConcurrentBacklog *v);

#endif
