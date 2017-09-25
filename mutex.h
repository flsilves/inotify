#ifndef INOTIFY_MUTEX_H
#define INOTIFY_MUTEX_H

using namespace std;

#include <pthread.h>


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

#endif