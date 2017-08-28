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




class concurrent_vector
{
    std::condition_variable cv;
    mutable std::mutex _mtx;
    std::vector<string> _vec;

public:

    string pop_back()
    {
        std::unique_lock<std::mutex> lk(_mtx);
        while (_vec.empty()) {
         cv.wait(lk);
        }
        assert(!_vec.empty());
        string ret = _vec.back();
        _vec.pop_back();
        return ret;
    }

    string pop_front()
    {
        std::unique_lock<std::mutex> lk(_mtx);
        string ret;
        while (_vec.empty()) {
         cv.wait(lk);
        }
        assert(!_vec.empty());
        ret = _vec.front();
        _vec.front() = std::move(_vec.back());
        _vec.pop_back();
        return ret;
    }

    void insert(string value)
    {
        std::lock_guard<std::mutex> lk(_mtx);
        _vec.insert(_vec.begin(), value);
        cv.notify_one();
    }



};
#endif
