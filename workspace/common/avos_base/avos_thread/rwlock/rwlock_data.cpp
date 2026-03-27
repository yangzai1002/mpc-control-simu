#include "rwlock_data.h"
#include "process_semaphore.h"
#include "thread_semaphore.h"

namespace avos
{

AvosRwLock::AvosRwLock(int type)
    : type(type), m_sem_name("")
{
    read = new ThreadSemaphore(1);
    read_count = new ThreadSemaphore(10000);
    write = new ThreadSemaphore(1);
    write_count = new ThreadSemaphore(10000);
    write_lock = new ThreadSemaphore(1);
    balance = new ThreadSemaphore(1);
}

AvosRwLock::AvosRwLock(int type, const std::string &sem_name)
    : type(type), m_sem_name(sem_name)
{
    read = new ProcessSemaphore(sem_name + "::read", 1);
    read_count = new ProcessSemaphore(sem_name + "::read_count", 10000);
    write = new ProcessSemaphore(sem_name + "::write", 1);
    write_count = new ProcessSemaphore(sem_name + "::write_count", 10000);
    write_lock = new ProcessSemaphore(sem_name + "::write_lock", 1);
    balance = new ProcessSemaphore(sem_name + "::balance", 1);
}

AvosRwLock::~AvosRwLock()
{
    auto FreeAndNil = [](RwlockSemaphore *&obj)
    {
        delete obj;
        obj = nullptr;
    };
    if (m_sem_name.empty())
    {
        FreeAndNil(read);
        FreeAndNil(read_count);
        FreeAndNil(write);
        FreeAndNil(write_count);
        FreeAndNil(write_lock);
        FreeAndNil(balance);
    }
}

} // namespace avos
