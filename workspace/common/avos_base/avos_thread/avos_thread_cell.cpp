#include "avos_thread_cell.h"
#include "avos_thread_name.h"
#include <iostream>

namespace avos
{

AvosThreadCell::AvosThreadCell(const std::string &name)
    : m_running(false), m_name(name)
{

}

AvosThreadCell::~AvosThreadCell()
{

}

void AvosThreadCell::start()
{
    if (m_running)
    {
        return;
    }
    AvosThreadName::set(m_name);
    m_running = true;
    this->do_run();
    m_running = false;
}

void AvosThreadCell::stop()
{
    if (m_running)
    {
        this->do_stop();
    }
    m_running = false;
}

} // namespace avos
