
#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <vector>
#include <semaphore.h>

class ThreadSafeSem {
public:
    ThreadSafeSem(int init_value = 0)
    {
        sem_init(&mSem, 0, init_value);
    }
    
    bool Wait()
    {
        return sem_wait(&mSem) == 0;
    }
    
    void setZero()
    {
        sem_init(&mSem, 0, 0);
    }

    long getSemNum()
    {
        return mSem.__align;
    }

    bool Post()
    {
        return sem_post(&mSem) == 0;
    }

    int sem_timedwait_millsecs(long msecs)
    {
        struct timespec ts;

        clock_gettime(CLOCK_REALTIME, &ts);
        long secs = msecs/1000;
        msecs = msecs%1000;

        long add = 0;
        msecs = msecs*1000*1000 + ts.tv_nsec;
        add = msecs / (1000*1000*1000);
        ts.tv_sec += (add + secs);
        ts.tv_nsec = msecs%(1000*1000*1000);
        return sem_timedwait(&mSem, &ts);
    }

private:
    sem_t mSem;
};

template<typename T>
class ThreadSafeQueue {
public:
    ThreadSafeQueue(int max_size = 10) {
        max_que_size = max_size;
    }

    ThreadSafeQueue(const ThreadSafeQueue& other) {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        m_dataQueue = other.m_dataQueue;
    }

    ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;

    void push(const T &value) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_dataQueue.push(value);
        if (m_dataQueue.size() > max_que_size) {
            m_dataQueue.pop();
        }
        m_cond.notify_one();
    }

    bool tryPop(T& value) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_dataQueue.empty()) {
            return false;
        }
        value = m_dataQueue.front();
        m_dataQueue.pop();
        return true;
    }


    void waitAndPop(T& value) {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (m_dataQueue.empty()) {
            m_cond.wait(lock);
        }
        value = m_dataQueue.front();
        m_dataQueue.pop();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_dataQueue.empty();
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_dataQueue.size();
    }


    // 获取最新的数据并清空队列
    bool getLatestAndClear(T& value) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_dataQueue.empty()) return false;

        // 把前面所有都弹掉，只留最后一个
        while (m_dataQueue.size() > 1) {
            m_dataQueue.pop();
        }
        value = m_dataQueue.front();  // 此时 front 就是最新的
        m_dataQueue.pop();
        return true;
    }

    // 获取最新的数据并清空队列
    std::vector<T> getLatestAndClear() {
        std::lock_guard<std::mutex> lock(m_mutex);


        std::vector<T> result;
        while (!m_dataQueue.empty()) {
            result.push_back(m_dataQueue.front());
            m_dataQueue.pop();
        }
        return result;
    }



    // 获取最新的N个数据并清空队列
    std::vector<T> getLatestNAndClear(size_t n) {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::vector<T> result;

        // 将队列数据转移到临时vector中
        std::vector<T> tempVector;
        while (!m_dataQueue.empty()) {
            tempVector.push_back(m_dataQueue.front());
            m_dataQueue.pop();
        }

        // 获取最新的N个数据（从vector末尾开始）
        size_t startIdx = (tempVector.size() > n) ? tempVector.size() - n : 0;
        for (size_t i = startIdx; i < tempVector.size(); ++i) {
            result.push_back(tempVector[i]);
        }

        return result;
    }

private:
    mutable std::mutex m_mutex;
    std::queue<T> m_dataQueue;
    std::condition_variable m_cond;
    int max_que_size;
};




template <typename T>
class ThreadSafeVector
{
    using DataChangeFuncType = std::function<void(const T &)>;


public:
    ThreadSafeVector(DataChangeFuncType on_push = nullptr, DataChangeFuncType on_delete = nullptr) : on_push_(on_push), on_delete_(on_delete)
    {
    }


    ThreadSafeVector(ThreadSafeVector&& other) noexcept
        : m_mutex()                                 // 重新构造新锁
        , on_push_(std::move(other.on_push_))
        , on_delete_(std::move(other.on_delete_))
        , data_(std::move(other.data_))
    {
    }

    void empty()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return data_.empty();
    }

    void push(const T &data)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (on_push_)
        {
            on_push_(data);
        }
        data_.push_back(data);
        m_cond.notify_one();
    }

    void clear()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (on_delete_)
        {
            for (size_t i = 0; i < data_.size(); i++)
            {
                on_delete_(data_[i]);
            }
        }
        data_.clear();
    }

    T& WaitAndGetLast()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (data_.size() == 0) {
            m_cond.wait(lock);
        }
        return data_.back();
    }

    ~ThreadSafeVector()
    {
        clear();
    }


private:
    mutable std::mutex m_mutex;
    DataChangeFuncType on_push_ = nullptr;
    DataChangeFuncType on_delete_ = nullptr;
    std::vector<T> data_;
    std::condition_variable m_cond;

};


template<typename T>
class ThreadSafeBuffer {
public:
    ThreadSafeBuffer() = default;
    ThreadSafeBuffer(const ThreadSafeBuffer&) = delete;
    ThreadSafeBuffer& operator=(const ThreadSafeBuffer&) = delete;

    /* 非阻塞版本：成功返回 0，失败返回 -1 */
    bool Get(std::shared_ptr<T>& out)        // 出参：深拷贝后的新指针
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_data) return false;

        out = std::make_shared<T>(*m_data); // 深拷贝
        // m_data.reset();                     // 把内部指针“移走”
        return true;
    }

    /* 阻塞版本：直到有数据才返回 */
    bool WaitForGet(std::shared_ptr<T>& out)
    {
        
        // m_cv.wait();
        m_sem.Wait();


        // std::unique_lock<std::mutex> lock(m_mutex);
        out = std::make_shared<T>(*m_data); // 深拷贝
        // m_data.reset();                     // 把内部指针“移走”
        return true;
    }

    /* 其余接口保持不变 */
    void Set(const std::shared_ptr<T>& d) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_data = d;
    }

    void SetNotify(const std::shared_ptr<T>& d) {
        // std::lock_guard<std::mutex> lock(m_mutex);
        m_data = d;
        // m_cv.notify_one();
        m_sem.Post();
    }

private:
    std::shared_ptr<T> m_data;
    std::mutex           m_mutex;
    // std::condition_variable m_cv;
    ThreadSafeSem m_sem;
};

#endif // THREAD_SAFE_QUEUE_H
