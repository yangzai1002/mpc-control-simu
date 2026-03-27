#ifndef AVOS_THREAD_H
#define AVOS_THREAD_H

#include <thread>
#include <string>

namespace avos
{

class AvosThreadSpin;

class AvosThread
{
protected:
    AvosThread(const std::string &name = "");
    virtual ~AvosThread();

    virtual void do_run() = 0;
    virtual void do_stop();

public:
    bool start();
    bool stop();
    bool is_start();

protected:
    void stopping();
    bool is_running();

private:
    void run();
    void stop_thread();

protected:
    std::string m_name;

private:
    bool m_running;
    int m_status;
    std::thread *m_thread;
    AvosThreadSpin *m_lock;
};

} // namespace avos

#endif
