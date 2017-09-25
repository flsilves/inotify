#include "mutex.h"


Mutex::Mutex() :
        isInitialized(false) {
}

Mutex::~Mutex() {
    terminate();
}


void Mutex::initialize() {
    if (isInitialized == false) {
        int rc = pthread_mutex_init(&mutex, NULL);
        if (rc != 0) {
           throw MyException("Failed to initialize Mutex");
        }
        isInitialized = true;
    }
}

void Mutex::lock() {
    int rc = pthread_mutex_lock(&mutex);
    if (rc != 0) {
        throw MyException("Failed to lock Mutex");
    }
}

void Mutex::unlock() {
    int rc = pthread_mutex_unlock(&mutex);
    if (rc != 0) {
        throw MyException("Failed to unlock Mutex");
    }

}

void Mutex::terminate() {
    if (true == isInitialized) {
        int rc = pthread_mutex_destroy(&mutex);
        if (rc != 0) {
            throw MyException("Failed to destroy Mutex");
        }
        isInitialized = false;
    }
}


