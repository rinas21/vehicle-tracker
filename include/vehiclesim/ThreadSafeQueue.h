#ifndef VEHICLESIM_THREADSAFEQUEUE_H
#define VEHICLESIM_THREADSAFEQUEUE_H

#include <condition_variable>
#include <mutex>
#include <queue>
#include <utility>

namespace vehiclesim {

template <typename T>
class ThreadSafeQueue {
public:
    ThreadSafeQueue() : closed_(false) {}

    void push(T value) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (closed_) {
                return;
            }
            queue_.push(std::move(value));
        }
        condition_.notify_one();
    }

    bool waitPop(T& value) {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [this] { return closed_ || !queue_.empty(); });

        if (queue_.empty()) {
            return false;
        }

        value = std::move(queue_.front());
        queue_.pop();
        return true;
    }

    void close() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            closed_ = true;
        }
        condition_.notify_all();
    }

    bool isClosed() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return closed_;
    }

private:
    mutable std::mutex mutex_;
    std::condition_variable condition_;
    std::queue<T> queue_;
    bool closed_;
};

} // namespace vehiclesim

#endif // VEHICLESIM_THREADSAFEQUEUE_H
