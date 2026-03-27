#include "process_semaphore.h"
#include "common_header/shared_memory.h"
#include <fcntl.h>
#include <sys/sem.h>
#include <condition_variable>
#include <unistd.h>

namespace avos
{

union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *arry;
};

ProcessSemaphore::ProcessSemaphore(const std::string &name, int value)
    : m_name(name), m_value(value), m_wait(new sembuf()), m_post(new sembuf())
{
    m_wait->sem_num = 0;
    m_wait->sem_op = -1;
    m_wait->sem_flg = SEM_UNDO;

    m_post->sem_num = 0;
    m_post->sem_op = 1;
    m_post->sem_flg = SEM_UNDO;

    key_t key = avos::common::SharedMemory::BKDRHash(m_name.c_str());
    m_id = semget(key, 1, 0640);
    if (m_id != -1 || errno != 2)
    {
        return;
    }
    m_id = semget(key, 1, 0640 | IPC_CREAT);
    if (m_id != -1)
    {
        semun sem;
        sem.val = m_value;
        semctl(m_id, 0, SETVAL, sem);
    }
}

ProcessSemaphore::~ProcessSemaphore()
{
    // semctl(m_id, 0, IPC_RMID);
    delete m_wait;
    delete m_post;
}

void ProcessSemaphore::wait()
{
    semop(m_id, m_wait, 1);
}

void ProcessSemaphore::wait_zero()
{
    int val = 0;
    do
    {
        val = semctl(m_id, 0, GETVAL);
        if (val < 0)
        {
            std::cout << "ProcessSemaphore::wait_zero() error\n";
        }
        usleep(1000*100);
    } while (val < m_value);
}

void ProcessSemaphore::post()
{
    semop(m_id, m_post, 1);
}

} // namespace avos
