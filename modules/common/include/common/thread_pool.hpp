#ifndef THREAD_POOL
#define THREAD_POOL

#include <functional>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <queue>
#include <future>
#include <atomic>

/**
 * @brief Thread-safe thread pool with configurable capacity
 */
class ThreadPool
{
public:
    /**
     * @brief Construct thread pool with specified number of threads
     * @param thread_num Number of worker threads
     * @param max_queue_size Maximum task queue size (0 = unlimited)
     */
    explicit ThreadPool(size_t thread_num, size_t max_queue_size = 0)
        : stop_(false), max_queue_size_(max_queue_size) {
        for(size_t i = 0; i < thread_num; ++i) {
            workers_.emplace_back([this]() {
                for(;;) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> ul(mtx_);
                        cv_.wait(ul, [this]() { return stop_.load() || !tasks_.empty(); });
                        if(stop_.load() && tasks_.empty()) {
                            return;
                        }
                        task = std::move(tasks_.front());
                        tasks_.pop();
                    }
                    cv_producer_.notify_one();  // Notify if queue was full
                    task();
                }
            });
        }
    }

    ~ThreadPool() {
        stop_.store(true);
        cv_.notify_all();
        for(auto& worker : workers_) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }

    /**
     * @brief Submit a task to the thread pool
     * @tparam F Function type
     * @tparam Args Argument types
     * @param f Function to execute
     * @param args Arguments to pass to the function
     * @return Future containing the result
     * @throws std::runtime_error if pool is stopped or queue is full
     */
    template<typename F, typename... Args>
    auto submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {
        using return_type = decltype(f(args...));

        auto taskPtr = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        {
            std::unique_lock<std::mutex> ul(mtx_);

            if(stop_.load()) {
                throw std::runtime_error("submit on stopped ThreadPool");
            }

            // Wait if queue is full (when max_queue_size_ > 0)
            if (max_queue_size_ > 0) {
                cv_producer_.wait(ul, [this]() {
                    return stop_.load() || tasks_.size() < max_queue_size_;
                });

                if(stop_.load()) {
                    throw std::runtime_error("submit on stopped ThreadPool");
                }
            }

            tasks_.emplace([taskPtr]() { (*taskPtr)(); });
        }
        cv_.notify_one();
        return taskPtr->get_future();
    }

    /**
     * @brief Get current number of pending tasks
     * @return Number of tasks in queue
     */
    size_t pending_tasks() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return tasks_.size();
    }

private:
    std::atomic<bool> stop_;
    size_t max_queue_size_;
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    mutable std::mutex mtx_;
    std::condition_variable cv_;
    std::condition_variable cv_producer_;  // For blocking when queue is full
};

#endif