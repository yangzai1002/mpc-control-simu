
#ifndef ANCILLARY_DEV_MEMS_CONCURRENTQUEUE_HPP
#define ANCILLARY_DEV_MEMS_CONCURRENTQUEUE_HPP

#include <condition_variable>
#include <deque>
#include <functional>
#include <limits>
#include <list>
#include <map>
#include <mutex>

namespace app_common {
    template <typename T> class ConcurrentQueue {
    protected:
        using DATATYPE = typename std::remove_cv<T>::type;
        using SIZETYPE = typename std::deque<DATATYPE>::size_type;

    public:
        explicit ConcurrentQueue(SIZETYPE limit = std::numeric_limits<SIZETYPE>::max()) : _limits(limit) {
        }

        ~ConcurrentQueue() = default;

        ConcurrentQueue(const ConcurrentQueue&) = delete;

        ConcurrentQueue& operator=(const ConcurrentQueue&) = delete;

        void push(const std::shared_ptr<DATATYPE>& d) {
            std::unique_lock<std::mutex> lk(_mtx);
            if (_queue.size() >= _limits &&
                std::cv_status::timeout == _cond_not_full.template wait_for(lk, std::chrono::milliseconds(10))) {
                throw std::runtime_error("full");
            }
            /*
             * 为什么要多这个判断呢？？？因为同步时钟可能会导致
             * _cond_not_empty.wait_for() 虚假地返回
             * https://en.cppreference.com/w/cpp/thread/condition_variable/wait_for:
             * It may also be unblocked spuriously.
             */
            if (_queue.size() >= _limits) {
                throw std::runtime_error("full");
            }
            _queue.push_back(d);
            aspect_run(_aspects_push, *d);
            if (1 == _queue.size()) {
                _cond_not_empty.notify_all();
            }
        }

        void push(std::shared_ptr<DATATYPE>&& d) {
            std::unique_lock<std::mutex> lk(_mtx);
            if (_queue.size() >= _limits &&
                std::cv_status::timeout == _cond_not_full.template wait_for(lk, std::chrono::milliseconds(10))) {
                throw std::runtime_error("full");
            }
            /*
             * 为什么要多这个判断呢？？？因为同步时钟可能会导致
             * _cond_not_empty.wait_for() 虚假地返回
             * https://en.cppreference.com/w/cpp/thread/condition_variable/wait_for:
             * It may also be unblocked spuriously.
             *
             */
            if (_queue.size() >= _limits) {
                throw std::runtime_error("full");
            }
            aspect_run(_aspects_push, *d);
            _queue.template emplace_back(std::move(d));
            if (1 == _queue.size()) {
                _cond_not_empty.notify_all();
            }
        }

        bool push_return_when_full(std::shared_ptr<DATATYPE>&& d) {
            std::unique_lock<std::mutex> lk(_mtx);
            if (_queue.size() >= _limits) {
                return false;
            }
            aspect_run(_aspects_push, *d);
            _queue.template emplace_back(std::move(d));
            if (1 == _queue.size()) {
                _cond_not_empty.notify_all();
            }
            return true;
        }

        void push(std::list<std::shared_ptr<DATATYPE>>& l) {
            std::unique_lock<std::mutex> lk(_mtx);
            if (_queue.size() >= _limits &&
                std::cv_status::timeout == _cond_not_full.template wait_for(lk, std::chrono::milliseconds(10))) {
                throw std::runtime_error("full");
            }
            /*
             * 为什么要多这个判断呢？？？因为同步时钟可能会导致
             * _cond_not_empty.wait_for() 虚假地返回
             * https://en.cppreference.com/w/cpp/thread/condition_variable/wait_for:
             * It may also be unblocked spuriously.
             *
             */
            if (_queue.size() >= _limits) {
                throw std::runtime_error("full");
            }

            auto old_size = _queue.size();
            while (!l.empty()) {
                auto& d = l.front();
                aspect_run(_aspects_push, *d);
                _queue.template emplace_back(std::move(d));
                l.pop_front();
            }

            if (0 == old_size) {
                _cond_not_empty.notify_all();
            }
        }

        bool push_return_when_full(std::list<std::shared_ptr<DATATYPE>>& l) {
            std::unique_lock<std::mutex> lk(_mtx);
            if (_queue.size() >= _limits) {
                return false;
            }

            auto old_size = _queue.size();
            while (!l.empty()) {
                auto& d = l.front();
                aspect_run(_aspects_push, *d);
                _queue.template emplace_back(std::move(d));
                l.pop_front();
            }

            if (0 == old_size) {
                _cond_not_empty.notify_all();
            }

            return true;
        }

        std::shared_ptr<DATATYPE> pop() {
            std::unique_lock<std::mutex> lk(_mtx);
            if (_queue.empty() &&
                std::cv_status::timeout == _cond_not_empty.template wait_for(lk, std::chrono::milliseconds(10))) {
                return nullptr;
            }
            /*
             * 为什么要多这个判断呢？？？因为同步时钟可能会导致
             * _cond_not_empty.wait_for() 虚假地返回
             * https://en.cppreference.com/w/cpp/thread/condition_variable/wait_for:
             * It may also be unblocked spuriously.
             *
             */
            if (_queue.empty()) {
                return nullptr;
            }

            auto former_size = _queue.size();
            auto p = _queue.front();
            aspect_run(_aspects_pop, *p);
            _queue.pop_front();
            if (former_size == _limits) {
                _cond_not_full.notify_all();
            }
            return p;
        }

        void aspect_add_for_push(const std::string& n, std::function<void(const DATATYPE&)>&& f) {
            std::lock_guard<std::mutex> g(_mtx);
            aspect_add(_aspects_push, n, std::move(f));
        }

        void aspect_del_for_push(const std::string& n) {
            std::lock_guard<std::mutex> g(_mtx);
            aspect_remove(_aspects_push, n);
        }

        void aspect_add_for_pop(const std::string& n, std::function<void(const DATATYPE&)>&& f) {
            std::lock_guard<std::mutex> g(_mtx);
            aspect_add(_aspects_pop, n, std::move(f));
        }

        void aspect_del_for_pop(const std::string& n) {
            std::lock_guard<std::mutex> g(_mtx);
            aspect_remove(_aspects_pop, n);
        }

        void flush() {
            std::lock_guard<std::mutex> g(_mtx);
            for (auto& x : _queue) {
                // _aspects_push 不需要 flush，因为在 push 的时候已经完成了
                aspect_run(_aspects_pop, *x);
            }
        }

    protected:
        void aspect_add(std::map<std::string, std::function<void(const DATATYPE&)>>& aspects, const std::string& n,
                        std::function<void(const DATATYPE&)>&& f) {
            aspects[n] = std::move(f);
        }
        void aspect_remove(std::map<std::string, std::function<void(const DATATYPE&)>>& aspects, const std::string& n) {
            aspects.erase(n);
        }
        void aspect_run(std::map<std::string, std::function<void(const DATATYPE&)>>& aspects, const DATATYPE& d) {
            for (const auto& entry : aspects) {
                entry.second(d);
            }
        }

    protected:
        const SIZETYPE                                              _limits;
        std::deque<std::shared_ptr<DATATYPE>>                       _queue;
        std::mutex                                                  _mtx;
        std::condition_variable                                     _cond_not_empty;
        std::condition_variable                                     _cond_not_full;
        std::map<std::string, std::function<void(const DATATYPE&)>> _aspects_push;
        std::map<std::string, std::function<void(const DATATYPE&)>> _aspects_pop;
    };
};     // namespace app_common
#endif // ANCILLARY_DEV_MEMS_CONCURRENTQUEUE_HPP
