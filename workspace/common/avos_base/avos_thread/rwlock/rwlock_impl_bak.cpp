#include "rwlock_impl.h"

#include <iostream>


#include <stdio.h>
#include <cstdlib>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

// struct AvosRwLockData
// {
//     char type;
//     std::string sem_name;
//     int read_count;
//     int write_count;
//     sem_t read;
//     sem_t write;
//     sem_t write_lock;
//     sem_t balance;
// };

namespace avos
{

RwLockIntf::RwLockIntf(AvosRwLockData *lock)
    : m_lock(lock)
{

}

RwLockIntf::~RwLockIntf()
{

}

void RwLockIntf::read()
{
    this->lock_read();
}

void RwLockIntf::write()
{
    this->lock_write();
}

void RwLockIntf::unlock()
{
    if (m_lock->state == 1)
    {
        this->unlock_read();
        return;
    }
    if (m_lock->state == 2)
    {
        this->unlock_write();
        return;
    }
}

RwLockBalanceImpl::RwLockBalanceImpl(AvosRwLockData *lock)
    : RwLockIntf(lock)
{

}

RwLockBalanceImpl::~RwLockBalanceImpl()
{

}

void RwLockBalanceImpl::lock_read()
{
    sem_wait(&m_lock->balance);
    sem_wait(&m_lock->read);
    if (m_lock->read_count == 0)
    {
        sem_wait(&m_lock->write);
    }
    m_lock->read_count++;
    sem_post(&m_lock->read);
    sem_post(&m_lock->balance);
    m_lock->state = 1;
}

void RwLockBalanceImpl::lock_write()
{
    sem_wait(&m_lock->balance);
    sem_wait(&m_lock->write);
    sem_post(&m_lock->balance);
    m_lock->state = 2;
}

void RwLockBalanceImpl::unlock_read()
{
    sem_wait(&m_lock->read);
    if (m_lock->read_count > 0)
    {
        m_lock->read_count--;
    }
    if (m_lock->read_count == 0)
    {
        m_lock->state = 0;
        sem_post(&m_lock->write);
    }
    sem_post(&m_lock->read);
}

void RwLockBalanceImpl::unlock_write()
{
    m_lock->state = 0;
    sem_post(&m_lock->write);
}

RwLockReadFirstImpl::RwLockReadFirstImpl(AvosRwLockData *lock)
    : RwLockIntf(lock)
{

}

RwLockReadFirstImpl::~RwLockReadFirstImpl()
{

}

void RwLockReadFirstImpl::lock_read()
{
    sem_wait(&m_lock->read);
    if (m_lock->read_count == 0)
    {
        sem_wait(&m_lock->write);
    }
    m_lock->read_count++;
    sem_post(&m_lock->read);
    m_lock->state = 1;
}

void RwLockReadFirstImpl::lock_write()
{
    sem_wait(&m_lock->write);
    m_lock->state = 2;
}

void RwLockReadFirstImpl::unlock_read()
{
    sem_wait(&m_lock->read);
    if (m_lock->read_count > 0)
    {
        m_lock->read_count--;
    }
    if (m_lock->read_count == 0)
    {
        m_lock->state = 0;
        sem_post(&m_lock->write);
    }
    sem_post(&m_lock->read);
}

void RwLockReadFirstImpl::unlock_write()
{
    m_lock->state = 0;
    sem_post(&m_lock->write);
}

RwLockWriteFirstImpl::RwLockWriteFirstImpl(AvosRwLockData *lock)
    : RwLockIntf(lock)
{

}

RwLockWriteFirstImpl::~RwLockWriteFirstImpl()
{

}

void RwLockWriteFirstImpl::lock_read()
{
    sem_wait(&m_lock->balance);
    sem_wait(&m_lock->read);
    if (m_lock->read_count == 0)
    {
        sem_wait(&m_lock->write);
    }
    m_lock->read_count++;
    sem_post(&m_lock->read);
    sem_post(&m_lock->balance);
    m_lock->state = 1;
}

void RwLockWriteFirstImpl::lock_write()
{
    sem_wait(&m_lock->write_lock);
    if (m_lock->write_count == 0)
    {
        sem_wait(&m_lock->balance);
    }
    m_lock->write_count++;
    sem_post(&m_lock->write_lock);
    sem_wait(&m_lock->write);
    m_lock->state = 2;
}

void RwLockWriteFirstImpl::unlock_read()
{
    sem_wait(&m_lock->read);
    if (m_lock->read_count > 0)
    {
        m_lock->read_count--;
    }
    if (m_lock->read_count == 0)
    {
        m_lock->state = 0;
        sem_post(&m_lock->write);
    }
    sem_post(&m_lock->read);
}

void RwLockWriteFirstImpl::unlock_write()
{
    sem_post(&m_lock->write);
    sem_wait(&m_lock->write_lock);
    if (m_lock->write_count > 0)
    {
        m_lock->write_count--;
    }
    if (m_lock->write_count == 0)
    {
        m_lock->state = 0;
        sem_post(&m_lock->balance);
    }
    sem_post(&m_lock->write_lock);
}

} // namespace avos
