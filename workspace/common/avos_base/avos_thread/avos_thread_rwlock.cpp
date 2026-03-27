#include "avos_thread_rwlock.h"
#include "rwlock/rwlock_impl.h"
#include "rwlock/rwlock_data.h"
#include "rwlock/process_semaphore.h"
#include "avos_shared_memory/avos_shared_memory.h"
#include "avos_utils/util.h"
#include <sys/file.h>

#include <iostream>

namespace avos
{

AvosThreadRwLock::AvosThreadRwLock(AvosRwLock *lock)
    : m_lock(nullptr)
{
    this->CreateLock(lock);
}

AvosThreadRwLock::~AvosThreadRwLock()
{
    if (m_lock)
    {
        delete m_lock;
    }
}

AvosRwLock *AvosThreadRwLock::CreateRwLock(thread::RwLockType type)
{
    AvosRwLock *lock = new AvosRwLock(type);
    return lock;
}

AvosRwLock *AvosThreadRwLock::CreateRwLock(thread::RwLockType type, const std::string &sem_name)
{
    std::string lock_name = sem_name + "::lock";
    common::Util::replace_all(lock_name, "/", "::");
    int fd;
    AvosSharedMemory::ErrorCode error;
    int *_type = (int *)AvosSharedMemory::GetMemory(lock_name, sizeof(int), true, fd, error);
    if (_type == nullptr)
    {
        return nullptr;
    }
    if (error == AvosSharedMemory::ErrorCode::smecCreate)
    {
        *_type = type;
    }
    flock(fd, LOCK_EX);
    AvosRwLock *lock = new AvosRwLock(*_type, lock_name);
    flock(fd, LOCK_UN);
    return lock;
}

void AvosThreadRwLock::DestoryRwLock(AvosRwLock *&lock)
{
    delete lock;
    lock = nullptr;
}

void AvosThreadRwLock::CreateLock(AvosRwLock *lock)
{
    switch (lock->type)
    {
    case thread::RwLockType::rwltNONE:
        m_lock = new RwLockNoneImpl(lock);
        break;
    case thread::RwLockType::rwltBALANCE:
        m_lock = new RwLockBalanceImpl(lock);
        break;
    case thread::RwLockType::rwltREADFIRST:
        m_lock = new RwLockReadFirstImpl(lock);
        break;
    case thread::RwLockType::rwltWRITEFIRST:
        m_lock = new RwLockWriteFirstImpl(lock);
        break;
    default:
        break;
    }
}

AvosThreadRead::AvosThreadRead(AvosRwLock *lock)
    : AvosThreadRwLock(lock)
{

}

AvosThreadRead::~AvosThreadRead()
{

}

void AvosThreadRead::lock()
{
    m_lock->lock_read();
}

void AvosThreadRead::unlock()
{
    m_lock->unlock_read();
}

AvosThreadWrite::AvosThreadWrite(AvosRwLock *lock)
    : AvosThreadRwLock(lock)
{

}

AvosThreadWrite::~AvosThreadWrite()
{

}

void AvosThreadWrite::lock()
{
    m_lock->lock_write();
}

void AvosThreadWrite::unlock()
{
    m_lock->unlock_write();
}

} // namespace avos
