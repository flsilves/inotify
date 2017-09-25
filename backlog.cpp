#include "backlog.h"

string ConcurrentBacklog::pop() {
    unique_lock<std::mutex> lock(_mutex);
    while (backlog.empty()) {
        _signalingCV.wait(lock);
    }
    assert(!backlog.empty());
    auto it = backlog.begin();
    string ret = *it;
    backlog.erase(it);
    return ret;
}

size_t ConcurrentBacklog::erase(string value) {
    unique_lock<std::mutex> lock(_mutex);
    return backlog.erase(value);
}

void ConcurrentBacklog::push(string value) {
    unique_lock<std::mutex> lock(_mutex);
    backlog.insert(backlog.begin(), value);
    _signalingCV.notify_one();
}

void ConcurrentBacklog::print(ostream& os) const {
    for (auto it = backlog.begin(); it != backlog.end(); ++it) {
        os << " " << *it << ",";
    }
}

int ConcurrentBacklog::size() const {
    return backlog.size();
}

void operator<<(ostream &os, ConcurrentBacklog *v) {
    v->print(os);
}
