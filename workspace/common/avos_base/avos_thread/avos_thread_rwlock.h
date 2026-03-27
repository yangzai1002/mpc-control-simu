#ifndef AVOS_THREAD_RWLOCK_H
#define AVOS_THREAD_RWLOCK_H

#include "avos_thread/thread_consts.h"
#include "avos_thread/avos_thread_lock.h"

namespace avos
{

class AvosRwLock;
class RwLockIntf;

class AvosThreadRwLock: public AvosThreadLock
{
protected:
    AvosThreadRwLock(AvosRwLock *lock);

public:
    virtual ~AvosThreadRwLock();

    static AvosRwLock *CreateRwLock(thread::RwLockType type);
    static AvosRwLock *CreateRwLock(thread::RwLockType type, const std::string &sem_name);
    static void DestoryRwLock(AvosRwLock *&lock);

private:
    void CreateLock(AvosRwLock *lock);

protected:
    RwLockIntf *m_lock;
};

class AvosThreadRead: public AvosThreadRwLock
{
public:
    AvosThreadRead(AvosRwLock *lock);
    ~AvosThreadRead();

    AvosThreadRead(const AvosThreadRead&) = delete;
    AvosThreadRead& operator=(const AvosThreadRead&) = delete;

    void lock() override;
    void unlock() override;
};

class AvosThreadWrite: public AvosThreadRwLock
{
public:
    AvosThreadWrite(AvosRwLock *lock);
    ~AvosThreadWrite();

    AvosThreadWrite(const AvosThreadWrite&) = delete;
    AvosThreadWrite& operator=(const AvosThreadWrite&) = delete;

    void lock() override;
    void unlock() override;
};

} // namespace avos

#endif
