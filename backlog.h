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

class ConcurrentSet {

private:
    std::condition_variable cv;
    std::mutex _mtx;
    std::set<string> _set;

public:
    string pop();
    size_t erase(string value);
    void push(string value);
};

ostream& operator<<(ostream &os, ConcurrentSet *v);



#endif // BACKLOG_H
