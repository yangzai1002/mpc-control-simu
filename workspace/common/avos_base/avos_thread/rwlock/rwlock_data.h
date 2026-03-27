#ifndef RWLOCK_DATA_H
#define RWLOCK_DATA_H

#include <string>

namespace avos
{

class RwlockSemaphore;

struct AvosRwLock
{
    int type;
    RwlockSemaphore *read;
    RwlockSemaphore *read_count;
    RwlockSemaphore *write;
    RwlockSemaphore *write_count;
    RwlockSemaphore *write_lock;
    RwlockSemaphore *balance;

    AvosRwLock(int type);
    AvosRwLock(int type, const std::string &sem_name);
    ~AvosRwLock();

private:
    std::string m_sem_name;
};

} // namespace avos

#endif
