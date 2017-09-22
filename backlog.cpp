#include "backlog.h"

concurrent_set::string pop() {
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

concurrent_set::size_t erase(string value) {
    std::unique_lock<std::mutex> lock(_mtx);
    return _set.erase(value);
}


concurrent_set:: void push(string value) {
    std::unique_lock<std::mutex> lock(_mtx); // RAII - Resource acquisition is initialization
    _set.insert(_set.begin(), value);
    cv.notify_one();
}

