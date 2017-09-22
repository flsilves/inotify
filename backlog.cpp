#include "backlog.h"

string concurrent_set::pop() {
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

size_t concurrent_set::erase(string value) {
    std::unique_lock<std::mutex> lock(_mtx);
    return _set.erase(value);
}

void concurrent_set::push(string value) {
    std::unique_lock<std::mutex> lock(_mtx); // RAII - Resource acquisition is initialization
    _set.insert(_set.begin(), value);
    cv.notify_one();
}

