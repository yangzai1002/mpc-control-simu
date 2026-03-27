#include "avos_thread.h"
#include "avos_thread_name.h"
#include "avos_thread_lock.h"
#include <iostream>

namespace avos
{

enum AvosThreadStatus
{
    atsStoped,
    atsStarted,
    atsFinished
};

AvosThread::AvosThread(const std::string &name)
    : m_name(name), m_running(false), m_status(AvosThreadStatus::atsStoped), m_thread(nullptr), m_lock(new AvosThreadSpin())
{

}

AvosThread::~AvosThread()
{
    this->stop();
    if (m_thread)
    {
        delete m_thread;
    }
    delete m_lock;
}

bool AvosThread::start()
{
    m_lock->lock();
    if (m_status == AvosThreadStatus::atsStarted)
    {
        m_lock->unlock();
        return true;
    }
    this->stop_thread();
    m_thread = new std::thread([&]() noexcept
    {
        AvosThreadName::set(m_name);
        this->run();
    });
    return true;
}

bool AvosThread::stop()
{
    AvosThreadSpinLocker locker(m_lock);
    if (m_status == AvosThreadStatus::atsStoped)
    {
        return true;
    }
    this->do_stop();
    this->stop_thread();
    m_status = AvosThreadStatus::atsStoped;
    return true;
}

void AvosThread::run()
{
    m_status = AvosThreadStatus::atsStarted;
    m_running = true;
    m_lock->unlock();
    this->do_run();
    m_running = false;
    if (m_status == AvosThreadStatus::atsStarted)
    {
        m_status = AvosThreadStatus::atsFinished;
    }
}

void AvosThread::do_stop()
{
    this->stopping();
}

void AvosThread::stop_thread()
{
    if (m_thread)
    {
        if (m_thread->joinable())
        {
            m_thread->join();
        }
        delete m_thread;
        m_thread = nullptr;
    }
}

void AvosThread::stopping()
{
    m_running = false;
}

bool AvosThread::is_running()
{
    return m_running;
}

bool AvosThread::is_start()
{
    return m_status == AvosThreadStatus::atsStarted;
}

} // namespace avos
