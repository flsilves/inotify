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

#include <sys/inotify.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>

using namespace std;

class concurrent_set
{
    std::condition_variable cv;
    std::mutex _mtx;
    std::set<string> _set;

public:

    string pop()
    {
        std::unique_lock<std::mutex> lock(_mtx); // RAII - Resource acquisition is initialization
        while (_set.empty()) {
         cv.wait(lock);
        }

        assert(!_set.empty());
        auto it = _set.begin();
        string ret = *it;
        _set.erase(it);
        return ret;
    }

    size_t erase(string value)
    {
        std::unique_lock<std::mutex> lock(_mtx);
        return _set.erase(value);
    }


    void push(string value)
    {
        std::unique_lock<std::mutex> lock(_mtx); // RAII - Resource acquisition is initialization
        _set.insert(_set.begin(), value);
        cv.notify_one();
    }
};

#endif // BACKLOG_H
