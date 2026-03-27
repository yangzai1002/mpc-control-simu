#include "avos_thread_pool.h"
#include "avos_thread_cell.h"
#include "avos_thread_name.h"
#include <iostream>

namespace avos
{

AvosThreadPool::AvosThreadPool(int count)
    : m_count(count), m_running(true)
{
    for (int i = 0; i < m_count; i++)
    {
        this->append_thread();
    }
}

AvosThreadPool::~AvosThreadPool()
{
    m_running = false;
    m_cond_todo.notify_all();
    for (std::thread &t : m_threads)
    {
        t.join();
    }
}

void AvosThreadPool::append_task(AvosThreadCell *task)
{
    std::unique_lock<std::mutex> locker(m_thread_mutex);
    m_tasks.push(task);
    locker.unlock();
    m_cond_todo.notify_one();
}

void AvosThreadPool::append_thread()
{
    m_threads.push_back(std::thread([&]() noexcept
    {
        AvosThreadName::set("thread pool");
        this->run_thread();
    }));
}

void AvosThreadPool::run_thread()
{
    while (m_running)
    {
        execute_task();
    }
}

void AvosThreadPool::execute_task()
{
    std::unique_lock<std::mutex> locker(m_thread_mutex);
    while (m_running && m_tasks.empty())
        m_cond_todo.wait(locker);
    if (m_running)
    {
        AvosThreadCell *task = m_tasks.front();
        m_tasks.pop();
        locker.unlock();
        task->start();
        delete task;
        AvosThreadName::set("thread pool");
    }
}

} // namespace avos
