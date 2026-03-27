#ifndef AVOS_THREAD_LOCK_H
#define AVOS_THREAD_LOCK_H

#include <mutex>
#include <condition_variable>
#include <atomic>

namespace avos
{

class AvosThreadLock
{
public:
    AvosThreadLock();
    virtual ~AvosThreadLock();

    virtual void lock() = 0;
    virtual void unlock() = 0;
};

class AvosThreadLocker
{
public:
    AvosThreadLocker(AvosThreadLock *locker);
    ~AvosThreadLocker();

private:
    AvosThreadLock *m_locker;
};

class AvosThreadMutex: public AvosThreadLock, public std::mutex
{
public:
    AvosThreadMutex();
    ~AvosThreadMutex();

    void lock() override;
    void unlock() override;
};

class AvosThreadSpin: public AvosThreadLock
{
public:
    AvosThreadSpin();
    ~AvosThreadSpin();

    AvosThreadSpin(const AvosThreadSpin&) = delete;
    AvosThreadSpin& operator=(const AvosThreadSpin&) = delete;

    void lock() override;
    void unlock() override;

private:
    std::atomic_flag m_lock;
};

typedef AvosThreadLocker AvosThreadMutexLocker;
typedef AvosThreadLocker AvosThreadSpinLocker;

} // namespace avos

#endif
