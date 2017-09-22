#include "backlog.h"

string ConcurrentSet::pop() {
    unique_lock<std::mutex> lock(_mtx);
    while (_set.empty()) {
        cv.wait(lock);
    }
    assert(!_set.empty());
    auto it = _set.begin();
    string ret = *it;
    _set.erase(it);
    return ret;
}

size_t ConcurrentSet::erase(string value) {
    unique_lock<std::mutex> lock(_mtx);
    return _set.erase(value);
}

void ConcurrentSet::push(string value) {
    unique_lock<std::mutex> lock(_mtx);
    _set.insert(_set.begin(), value);
    cv.notify_one();
}

