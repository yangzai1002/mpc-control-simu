#ifndef RWLOCK_SEMAPHORE_H
#define RWLOCK_SEMAPHORE_H

namespace avos
{

class RwlockSemaphore
{
public:
    virtual ~RwlockSemaphore() {};
    virtual void wait() = 0;
    virtual void wait_zero() = 0;
    virtual void post() = 0;
};

} // namespace avos

#endif
