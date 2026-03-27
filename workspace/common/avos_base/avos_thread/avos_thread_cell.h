#ifndef AVOS_THREAD_CELL_H
#define AVOS_THREAD_CELL_H

#include <string>

namespace avos
{

class AvosThreadCell
{
    friend class AvosThread;
    friend class AvosThreadPool;

protected:
    AvosThreadCell(const std::string &name = "");
    virtual ~AvosThreadCell();

    virtual void do_run() = 0;
    virtual void do_stop() = 0;

private:
    void start();
    void stop();

protected:
    bool m_running;

private:
    std::string m_name;
};

} // namespace avos

#endif
