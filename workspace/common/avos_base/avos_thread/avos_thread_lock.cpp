#include "avos_thread_lock.h"
#include <iostream>

namespace avos
{

AvosThreadLock::AvosThreadLock()
{

}

AvosThreadLock::~AvosThreadLock()
{

}

AvosThreadLocker::AvosThreadLocker(AvosThreadLock *locker)
    : m_locker(locker)
{
    m_locker->lock();
}

AvosThreadLocker::~AvosThreadLocker()
{
    m_locker->unlock();
}

AvosThreadMutex::AvosThreadMutex()
    : std::mutex()
{

}

AvosThreadMutex::~AvosThreadMutex()
{

}

void AvosThreadMutex::lock()
{
    std::mutex::lock();
}

void AvosThreadMutex::unlock()
{
    std::mutex::unlock();
}

AvosThreadSpin::AvosThreadSpin()
    : m_lock(ATOMIC_FLAG_INIT)
{

}

AvosThreadSpin::~AvosThreadSpin()
{

}

void AvosThreadSpin::lock()
{
    while (std::atomic_flag_test_and_set_explicit(&m_lock, std::memory_order_acquire));
}

void AvosThreadSpin::unlock()
{
    std::atomic_flag_clear_explicit(&m_lock, std::memory_order_release);
}

} // namespace avos
