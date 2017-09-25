#include <pthread.h>

using namespace std;

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
            l
        }
        isInitialized = true;
    }
}

void Mutex::lock() {
    int rc = pthread_mutex_lock(&mutex);
    if (rc != 0) {

    }
}

void Mutex::unlock() {
    int rc = pthread_mutex_unlock(&mutex);
    if (rc != 0) {

    }

}

void Mutex::terminate() {
    if (true == isInitialized) {
        int rc = pthread_mutex_destroy(&mutex);
        if (rc != 0) {

        }
        isInitialized = false;
    }
}
