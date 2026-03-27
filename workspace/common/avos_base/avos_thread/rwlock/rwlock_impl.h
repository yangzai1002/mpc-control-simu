#ifndef RWLOCK_IMPL_H
#define RWLOCK_IMPL_H

namespace avos
{

class AvosRwLock;

class RwLockIntf
{
protected:
    RwLockIntf(AvosRwLock *lock);

public:
    virtual ~RwLockIntf();

    virtual void lock_read() = 0;
    virtual void lock_write() = 0;
    virtual void unlock_read() = 0;
    virtual void unlock_write() = 0;

protected:
    AvosRwLock *m_lock;
};

class RwLockNoneImpl: public RwLockIntf
{
public:
    RwLockNoneImpl(AvosRwLock *lock);
    ~RwLockNoneImpl();

private:
    void lock_read() override;
    void lock_write() override;
    void unlock_read() override;
    void unlock_write() override;
};

class RwLockBalanceImpl: public RwLockIntf
{
public:
    RwLockBalanceImpl(AvosRwLock *lock);
    ~RwLockBalanceImpl();

private:
    void lock_read() override;
    void lock_write() override;
    void unlock_read() override;
    void unlock_write() override;
};

class RwLockReadFirstImpl: public RwLockIntf
{
public:
    RwLockReadFirstImpl(AvosRwLock *lock);
    ~RwLockReadFirstImpl();

private:
    void lock_read() override;
    void lock_write() override;
    void unlock_read() override;
    void unlock_write() override;
};

class RwLockWriteFirstImpl: public RwLockIntf
{
public:
    RwLockWriteFirstImpl(AvosRwLock *lock);
    ~RwLockWriteFirstImpl();

private:
    void lock_read() override;
    void lock_write() override;
    void unlock_read() override;
    void unlock_write() override;
};

} // namespace avos

#endif
