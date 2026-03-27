#include "thread_semaphore.h"
#include <fcntl.h>

namespace avos
{

ThreadSemaphore::ThreadSemaphore(int value)
    : m_sem(new sem_t())
{
    sem_init(m_sem, 1, value);
}

// ThreadSemaphore::ThreadSemaphore(const std::string &name, int value)
//     : m_name(name)
// {
//     m_sem = sem_open(name.c_str(), O_RDWR, S_IRUSR | S_IWUSR, value);
//     if (m_sem == SEM_FAILED)
//     {
//         m_sem = sem_open(name.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, value);
//     }
// }

ThreadSemaphore::~ThreadSemaphore()
{
    sem_destroy(m_sem);
    delete m_sem;
}

void ThreadSemaphore::wait()
{
    sem_wait(m_sem);
}

void ThreadSemaphore::wait_zero()
{
    int val = 0;
    do
    {
        sem_getvalue(m_sem, &val);
    } while (val < 10000);
}

void ThreadSemaphore::post()
{
    sem_post(m_sem);
}

} // namespace avos
