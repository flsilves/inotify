#include "backlog.h"


ConcurrentBacklog::ConcurrentBacklog()  {
    pMutex.initialize();

}

ConcurrentBacklog::~ConcurrentBacklog() {
    pMutex.terminate();
}

string ConcurrentBacklog::pop() {
    MutexAutoLock lk(pMutex);

   // if (backlog.empty()) {
    //return nullptr ;    //notEmptyCondition.wait(lk);
   // }

    assert(!backlog.empty());
    auto it = backlog.begin();
    string ret = *it;
    backlog.erase(it);
    return ret;
}

size_t ConcurrentBacklog::erase(string value) {
    MutexAutoLock lk(pMutex);
    return backlog.erase(value);
}

void ConcurrentBacklog::push(string value) {
    MutexAutoLock lk(pMutex);
    backlog.insert(backlog.begin(), value);
    //notEmptyCondition.notify_one();
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
