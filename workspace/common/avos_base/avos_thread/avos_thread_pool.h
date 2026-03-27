#ifndef AVOS_THREAD_POOL_H
#define AVOS_THREAD_POOL_H

#include <condition_variable>
#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <functional>

namespace avos
{

class AvosThreadCell;

class AvosThreadPool
{
public:
    AvosThreadPool(int count);
    ~AvosThreadPool();

    void append_task(AvosThreadCell *task);

private:
    void append_thread();
    void run_thread();
    void execute_task();

private:
    int m_count;
    bool m_running;
    std::condition_variable m_cond_todo;
    std::mutex m_thread_mutex;
    std::mutex m_task_mutex;
    std::vector<std::thread> m_threads;
    std::queue<AvosThreadCell *> m_tasks;
};

} // namespace avos

#endif
