#ifndef BACKLOG_H
#define BACKLOG_H

#include "myTimer.h"

#include <iostream>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <cstdlib>

#include <dirent.h>
#include <cstdarg>
#include <cassert>
#include <cstring>
#include <set>

#include <sys/inotify.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>

using namespace std;

class ConcurrentBacklog {

private:
    std::condition_variable notEmptyCV;
    std::mutex writeMutex;
    std::set<string> backlog;

public:
    void print(ostream &os = std::cout) const;
    int size() const;
    string pop();
    size_t erase(string value);
    void push(string value);
};

void operator<<(ostream &os, ConcurrentBacklog *v);

#endif
