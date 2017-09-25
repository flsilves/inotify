#include "backlog.h"

string ConcurrentBacklog::pop() {
    unique_lock<std::mutex> lock(writeMutex);

    while (backlog.empty()) {
        notEmptyCondition.wait(lock);
    }

    assert(!backlog.empty());
    auto it = backlog.begin();
    string ret = *it;
    backlog.erase(it);
    return ret;
}

size_t ConcurrentBacklog::erase(string value) {
    unique_lock<std::mutex> lock(writeMutex);
    return backlog.erase(value);
}

void ConcurrentBacklog::push(string value) {
    unique_lock<std::mutex> lock(writeMutex);
    backlog.insert(backlog.begin(), value);
    notEmptyCondition.notify_one();
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
