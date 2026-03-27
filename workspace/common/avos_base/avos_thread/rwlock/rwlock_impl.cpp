#include "rwlock_impl.h"

#include "rwlock_data.h"
#include "process_semaphore.h"

namespace avos
{

RwLockIntf::RwLockIntf(AvosRwLock *lock)
    : m_lock(lock)
{

}

RwLockIntf::~RwLockIntf()
{

}

RwLockNoneImpl::RwLockNoneImpl(AvosRwLock *lock)
    : RwLockIntf(lock)
{

}

RwLockNoneImpl::~RwLockNoneImpl()
{

}

void RwLockNoneImpl::lock_read()
{

}

void RwLockNoneImpl::lock_write()
{

}

void RwLockNoneImpl::unlock_read()
{

}

void RwLockNoneImpl::unlock_write()
{

}

RwLockBalanceImpl::RwLockBalanceImpl(AvosRwLock *lock)
    : RwLockIntf(lock)
{

}

RwLockBalanceImpl::~RwLockBalanceImpl()
{

}

void RwLockBalanceImpl::lock_read()
{
    m_lock->balance->wait();
    m_lock->read_count->wait();
    m_lock->write_count->wait_zero();
    m_lock->balance->post();
}

void RwLockBalanceImpl::lock_write()
{
    m_lock->balance->wait();
    m_lock->write_count->wait();
    m_lock->read_count->wait_zero();
    m_lock->write->wait();
    m_lock->balance->post();
}

void RwLockBalanceImpl::unlock_read()
{
    m_lock->read_count->post();
}

void RwLockBalanceImpl::unlock_write()
{
    m_lock->write->post();
    m_lock->write_count->post();
}

RwLockReadFirstImpl::RwLockReadFirstImpl(AvosRwLock *lock)
    : RwLockIntf(lock)
{

}

RwLockReadFirstImpl::~RwLockReadFirstImpl()
{

}

void RwLockReadFirstImpl::lock_read()
{
    m_lock->read_count->wait();
    m_lock->write_count->wait_zero();
}

void RwLockReadFirstImpl::lock_write()
{
    m_lock->read_count->wait_zero();
    m_lock->write_count->wait();
    m_lock->write->wait();
}

void RwLockReadFirstImpl::unlock_read()
{
    m_lock->read_count->post();
}

void RwLockReadFirstImpl::unlock_write()
{
    m_lock->write->post();
    m_lock->write_count->post();
}

RwLockWriteFirstImpl::RwLockWriteFirstImpl(AvosRwLock *lock)
    : RwLockIntf(lock)
{

}

RwLockWriteFirstImpl::~RwLockWriteFirstImpl()
{

}

void RwLockWriteFirstImpl::lock_read()
{
    m_lock->write_count->wait_zero();
    m_lock->read_count->wait();
}

void RwLockWriteFirstImpl::lock_write()
{
    m_lock->write_lock->wait();
    m_lock->write_count->wait();
    m_lock->read_count->wait_zero();
    m_lock->write_lock->post();
    m_lock->write->wait();
}

void RwLockWriteFirstImpl::unlock_read()
{
    m_lock->read_count->post();
}

void RwLockWriteFirstImpl::unlock_write()
{
    m_lock->write->post();
    m_lock->write_count->post();
}

} // namespace avos
