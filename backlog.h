#ifndef BACKLOG_H
#define BACKLOG_H

#include <stdlib.h>
#include <stdio.h>
#include <linux/limits.h>

#include <condition_variable>
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
#include <set>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include "myTimer.h"
#include <stdarg.h>  

using namespace std;


template <typename T>
class concurrent_set
{
private:
    set::set<T> set_;
    std::mutex mutex_;

public:
    typedef typename std::set<T>::iterator iterator;

    insert(const T& val) {
        std::unique_lock<std::mutex> lock(mutex_);
        return set_.insert(val);
    }

    size_type size() const {
        std::unique_lock<std::mutex> lock(mutex_);
        return set_.size();
    }

    size_type erase(const T& val)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        return set_.erase(val);
    }

};

#endif
