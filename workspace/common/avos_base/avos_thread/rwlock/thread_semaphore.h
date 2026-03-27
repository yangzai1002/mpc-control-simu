#ifndef THREAD_SEMAPHORE_H
#define THREAD_SEMAPHORE_H

#include "rwlock_semaphore.h"
#include <semaphore.h>

namespace avos
{

class ThreadSemaphore: public virtual RwlockSemaphore
{
public:
    ThreadSemaphore(int value);
    // ThreadSemaphore(const std::string &name, int value);
    ~ThreadSemaphore();

private:
    void wait() override;
    void wait_zero() override;
    void post() override;

private:
    sem_t *m_sem;
};

} // namespace avos

#endif
