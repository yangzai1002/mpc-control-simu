#ifndef PROCESS_SEMAPHORE_H
#define PROCESS_SEMAPHORE_H

#include "rwlock_semaphore.h"
#include <string>

struct sembuf;

namespace avos
{

class ProcessSemaphore: public virtual RwlockSemaphore
{
public:
    ProcessSemaphore(const std::string &name, int value);
    ~ProcessSemaphore();

private:
    void wait() override;
    void wait_zero() override;
    void post() override;

private:
    int m_id;
    std::string m_name;
    int m_value;
    sembuf *m_wait;
    sembuf *m_post;
};

} // namespace avos

#endif
