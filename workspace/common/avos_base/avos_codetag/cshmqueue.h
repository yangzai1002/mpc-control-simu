#ifndef LOCK_FREE_QUEUE_H
#define LOCK_FREE_QUEUE_H
#include <iostream>
#include <vector>
#include <atomic>
#include <thread>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <shared_mutex>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */

struct SharedHeader {
    std::atomic<std::size_t> write_index;
    std::atomic<std::size_t> buf_len;
    std::atomic<std::size_t> write_count;
    std::atomic<std::size_t> recover[5];
    //    mutable std::shared_mutex mutex_; // 用于保护读写操作的同步锁
    SharedHeader()
    {}
};

// Define the circular queue's data structure
template<typename T, std::size_t N>
class LockFreeQueue {
public:
//    LockFreeQueue(T* buffer) : buffer_(buffer)
    LockFreeQueue()
    {
    }

    void init() {
        mheader_.buf_len.store(N);
        mheader_.write_count.store(0);
    }

    bool write(const T& value)
    {
        // int size=sizeof(SharedHeader);
        // int current_tail = mheader_.write_index.load();
        // int next_tail = increment(current_tail);
        // std::cout <<mheader_.write_count.load(std::memory_order_relaxed) <<std::endl;
        // mheader_.write_index.store(next_tail);
        int current_tail = mheader_.write_count.fetch_add(1);
        buffer_[(current_tail) & (N - 1)] = value;
        return true;
    }

    bool read(T& value,int index)
    {
        if(index < N-1)
        {
            value = buffer_[index];
            return true;
        }
        else
        {
            return false;
        }
    }
    //only be used when N = 2^n
    std::size_t increment(std::size_t index) const
    {
        return (index + 1) & (N - 1);
    }

    SharedHeader const * getheader()
    {
        return &mheader_;
    }
//    void setbufaddr(intptr_t addr)
//    {
//        printf("addr = 0x%x\n",addr);
//        buffer_ = (T*)(void*)addr;
//    }
private:
    SharedHeader mheader_;

    T buffer_[0];


};


#endif // LOCK_FREE_QUEUE_H
