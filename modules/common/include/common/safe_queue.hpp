/*
 * @Author: aurson jassimxiong@gmail.com
 * @Date: 2025-05-24 12:17:47
 * @LastEditors: aurson jassimxiong@gmail.com
 * @LastEditTime: 2025-06-14 13:39:52
 * @Description: Thread-safe queue with data stored on heap, suitable for large data scenarios like image processing
 * Copyright (c) 2025 by Aurson, All Rights Reserved.
 */
#ifndef __SAFE_QUEUE_H__
#define __SAFE_QUEUE_H__

#include <queue>
#include <mutex>
#include <condition_variable>

/**
 * @brief Thread-safe queue with capacity limit
 * @tparam T Type of elements stored in the queue
 */
template <typename T>
class SafeQueue
{
public:
    explicit SafeQueue(size_t cap = 200)
        : capacity_(cap),
          stopped_(false)
    {
    }

    ~SafeQueue()
    {
        stop();
    }

    /**
     * @brief Push element to queue
     * @param value Element to push
     * @return true if push succeeded, false if queue is full
     */
    bool push(const T &value)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.size() >= capacity_)
        {
            return false;
        }

        queue_.push(value);
        cv_.notify_one();
        return true;
    }

    /**
     * @brief Try to pop element from queue (non-blocking)
     * @param value Output parameter for popped element
     * @return true if pop succeeded, false if queue is empty
     */
    bool try_pop(T &value)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty())
        {
            return false;
        }
        value = std::move(queue_.front());
        queue_.pop();
        return true;
    }

    /**
     * @brief Pop element from queue (blocking)
     * @param value Output parameter for popped element
     * @return true if pop succeeded, false if stopped
     * @note Blocks until data is available or stop() is called
     */
    bool pop(T &value)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this]() { return !queue_.empty() || stopped_; });

        if (stopped_)
        {
            return false;
        }

        value = std::move(queue_.front());
        queue_.pop();
        return true;
    }

    /**
     * @brief Stop queue operations
     */
    void stop()
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (stopped_)
            {
                return;
            }
            stopped_ = true;
        }
        cv_.notify_all();
    }

    /**
     * @brief Clear all elements in queue
     */
    void clear()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        while (!queue_.empty())
        {
            queue_.pop();
        }
    }

    /**
     * @brief Get current queue size
     * @return Number of elements in queue
     */
    size_t size() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

    /**
     * @brief Check if queue is empty
     * @return true if empty, false otherwise
     */
    bool empty() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

private:
    size_t capacity_;
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    bool stopped_;
};

#endif // __SAFE_QUEUE_H__