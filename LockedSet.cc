#ifndef LOCKED_SET_H
#define LOCKED_SET_H

#include <set>
#include <thread>
#include <mutex>
 
template <typename T>
class LockedSet {
private:
    std::set<T> m_set;
    std::mutex Mutex;
    friend class LockableSetLock;
};

class LockableSetLock {
private:
    LockedSet& set_;
public:
    LockableSetLock(LockedSet& set) : set_(set) { set.Mutex.Lock(); }
    ~LockableSetLock(){ set.Mutex.Unlock(); }
}