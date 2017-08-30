#ifndef BACKLOG_H
#define BACKLOG_H

#include "myTimer.h"

#include <iostream>

#include <thread>
#include <mutex>
#include <condition_variable>

#include <dirent.h> 
#include <stdarg.h>  
#include <cassert>

#include <sys/inotify.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>

using namespace std;

class concurrent_set
{
    std::condition_variable cv;
    mutable std::mutex _mtx;
    std::set<string> _set;

public:

    string pop()
    {
        std::unique_lock<std::mutex> lk(_mtx);
        while (_set.empty()) {
         cv.wait(lk);
        }
        assert(!_set.empty());
        auto it = _set.begin();
        string ret = *it;
        _set.erase(it);
        return ret;
    }


    void insert(string value)
    {
        std::unique_lock<std::mutex> lk(_mtx);
        _set.insert(_set.begin(), value);
        cv.notify_one();
    }
};

#endif
