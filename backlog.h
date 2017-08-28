#ifndef BACKLOG_H
#define BACKLOG_H

#include <stdlib.h>
#include <stdio.h>
#include <linux/limits.h>

#include <condition_variable>
#include <cassert>
#include <thread>
#include <mutex>
#include <iostream>
#include <sys/inotify.h>
#include <dirent.h> 
#include <stdio.h> 
#include <iostream>
#include <limits.h> 
#include <thread>
#include <unistd.h>
#include <string.h>
#include <algorithm>
#include <vector>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include "myTimer.h"
#include <stdarg.h>  
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

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
