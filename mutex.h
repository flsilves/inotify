#ifndef INOTIFY_MUTEX_H
#define INOTIFY_MUTEX_H

#include <pthread.h>
#include "exception.h"

class Mutex {
    friend class ConcurrentBacklog;

public:
    Mutex();

    ~Mutex();

    void initialize();

    void lock();

    void unlock();

    void terminate();


private:
    Mutex(const Mutex &m);

    pthread_mutex_t mutex;
    bool isInitialized;

};

class MutexAutoLock {

    friend class ConcurrentBacklog;

public:

    MutexAutoLock(Mutex &m) : mutex(m) {
        mutex.lock();
    }

    ~MutexAutoLock() {
        mutex.unlock();
    }

private:
    MutexAutoLock(const MutexAutoLock &m);

    Mutex &mutex;
};

#endif