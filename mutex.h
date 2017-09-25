#ifndef INOTIFY_MUTEX_H
#define INOTIFY_MUTEX_H



#include <pthread.h>
#include "exception.h"

class Mutex
{
public:
    Mutex();
    ~Mutex();

    void initialize();
    void lock();
    void unlock();
    void terminate();

private:
    Mutex(const Mutex &m);

    bool isInitialized;
    pthread_mutex_t mutex;
};

class MutexAutoLock
{
public:

    MutexAutoLock(Mutex &m) : mutex(m)
    {
        mutex.lock();
    }

    ~MutexAutoLock()
    {
        mutex.unlock();
    }

private:
    MutexAutoLock(const MutexAutoLock &m);

    Mutex &mutex;
};

#endif