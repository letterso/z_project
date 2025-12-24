#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdio.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <thread>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <functional>
#include <chrono>
#include <cerrno>
#include <iostream>

/**
 * @brief RAII wrapper for file descriptors
 */
class FileDescriptor {
public:
    FileDescriptor() : fd_(-1) {}
    explicit FileDescriptor(int fd) : fd_(fd) {}

    ~FileDescriptor() {
        close();
    }

    // Delete copy operations
    FileDescriptor(const FileDescriptor&) = delete;
    FileDescriptor& operator=(const FileDescriptor&) = delete;

    // Move operations
    FileDescriptor(FileDescriptor&& other) noexcept : fd_(other.fd_) {
        other.fd_ = -1;
    }

    FileDescriptor& operator=(FileDescriptor&& other) noexcept {
        if (this != &other) {
            close();
            fd_ = other.fd_;
            other.fd_ = -1;
        }
        return *this;
    }

    int get() const { return fd_; }
    bool is_valid() const { return fd_ != -1; }

    void reset(int fd = -1) {
        close();
        fd_ = fd;
    }

    int release() {
        int fd = fd_;
        fd_ = -1;
        return fd;
    }

private:
    void close() {
        if (fd_ != -1) {
            ::close(fd_);
            fd_ = -1;
        }
    }

    int fd_;
};

/**
 * @brief High-precision timer using timerfd + epoll
 */
class Timer {
public:
    using TaskCallback = std::function<void(void)>;

    Timer(uint64_t ms, const TaskCallback& callback)
        : is_running_(false)
        , interval_ms_(ms)
        , callback_(callback) {}

    ~Timer() {
        stop();
    }

    void start() {
        if (is_running_.load()) {
            return;
        }

        // Create timerfd
        timer_fd_.reset(timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC));
        if (!timer_fd_.is_valid()) {
            throw std::system_error(errno, std::generic_category(), "timerfd_create");
        }

        // Create wakeup eventfd
        wakeup_fd_.reset(eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC));
        if (!wakeup_fd_.is_valid()) {
            throw std::system_error(errno, std::generic_category(), "eventfd");
        }

        // Set timer interval
        struct itimerspec its;
        memset(&its, 0, sizeof(its));
        uint64_t ms = interval_ms_;
        its.it_interval.tv_sec = ms / 1000;
        its.it_interval.tv_nsec = (ms % 1000) * 1000000;
        its.it_value = its.it_interval;
        if (timerfd_settime(timer_fd_.get(), 0, &its, nullptr) == -1) {
            throw std::system_error(errno, std::generic_category(), "timerfd_settime");
        }

        // Create epoll instance
        epoll_fd_.reset(epoll_create1(EPOLL_CLOEXEC));
        if (!epoll_fd_.is_valid()) {
            throw std::system_error(errno, std::generic_category(), "epoll_create1");
        }

        // Add timer_fd to epoll
        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = timer_fd_.get();
        if (epoll_ctl(epoll_fd_.get(), EPOLL_CTL_ADD, timer_fd_.get(), &ev) == -1) {
            throw std::system_error(errno, std::generic_category(), "epoll_ctl add timer_fd");
        }

        // Add wakeup_fd to epoll
        ev.data.fd = wakeup_fd_.get();
        if (epoll_ctl(epoll_fd_.get(), EPOLL_CTL_ADD, wakeup_fd_.get(), &ev) == -1) {
            throw std::system_error(errno, std::generic_category(), "epoll_ctl add wakeup_fd");
        }

        is_running_.store(true);

        // Start worker thread
        thread_ = std::thread([this]() {
            const int MAX_EVENTS = 2;
            struct epoll_event events[MAX_EVENTS];

            while (is_running_.load()) {
                int nfds = epoll_wait(epoll_fd_.get(), events, MAX_EVENTS, -1);
                if (nfds == -1) {
                    if (errno == EINTR) {
                        continue;
                    }
                    std::cerr << "[TIMER] epoll_wait error: " << strerror(errno) << std::endl;
                    break;
                }

                for (int i = 0; i < nfds; ++i) {
                    if (events[i].data.fd == timer_fd_.get()) {
                        uint64_t expirations;
                        ssize_t bytes = read(timer_fd_.get(), &expirations, sizeof(expirations));
                        if (bytes == sizeof(expirations) && expirations > 0) {
                            for (uint64_t j = 0; j < expirations; ++j) {
                                if (callback_) {
                                    callback_();
                                }
                            }
                        }
                    } else if (events[i].data.fd == wakeup_fd_.get()) {
                        // Read wakeup signal and exit
                        uint64_t val;
                        read(wakeup_fd_.get(), &val, sizeof(val));
                        return;
                    }
                }
            }
        });
    }

    void stop() {
        if (!is_running_.exchange(false)) {
            return;
        }

        // Send wakeup signal
        if (wakeup_fd_.is_valid()) {
            uint64_t val = 1;
            ssize_t ret = write(wakeup_fd_.get(), &val, sizeof(val));
            if (ret == -1) {
                std::cerr << "[TIMER] Failed to write wakeup signal: " << strerror(errno) << std::endl;
            }
        }

        if (thread_.joinable()) {
            thread_.join();
        }

        // RAII will automatically close all file descriptors
    }

private:
    std::atomic<bool> is_running_;
    FileDescriptor timer_fd_;
    FileDescriptor epoll_fd_;
    FileDescriptor wakeup_fd_;
    std::thread thread_;
    uint64_t interval_ms_;
    TaskCallback callback_;
};

#endif // __TIMER_H__

// poll 实现
/**
#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdio.h>
#include <sys/poll.h>
#include <sys/timerfd.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <thread>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <functional>

class Timer {
public:
    using TaskCallback = std::function<void(void)>;
    Timer(uint64_t ms, const TaskCallback &callback)
    : m_is_running(false)
    , m_timer_fd(-1)
    , m_event_fd(-1)
    , m_interval_ms(ms)
    , m_callback(callback) {
    }
    ~Timer() {
        stop();
    }

    bool start() {
        if (m_is_running) {
            return true;
        }
    
        // 创建eventfd用于停止信号
        m_event_fd = eventfd(0, EFD_NONBLOCK);
        if (m_event_fd == -1) {
            perror("Failed to create eventfd");
            return false;
        }

        // 创建定时器文件描述符
        m_timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
        if (m_timer_fd == -1) {
            perror("Failed to create timerfd");
            return false;
        }

        // 设置定时器参数
        struct itimerspec its;
        memset(&its, 0, sizeof(its));
        its.it_interval.tv_sec = m_interval_ms / 1000;
        its.it_interval.tv_nsec = (m_interval_ms % 1000) * 1000000;
        its.it_value = its.it_interval; // 首次超时时间与间隔相同

        if (timerfd_settime(m_timer_fd, 0, &its, nullptr) == -1) {
            perror("Failed to set timerfd time");
            close(m_timer_fd);
            close(m_event_fd);
            m_timer_fd = -1;
            m_event_fd = -1;
            return false;
        }

        m_is_running.store(true);
        m_thread = std::thread([this]() {
            // 提升线程优先级（需要权限）
            // struct sched_param param;
            // param.sched_priority = sched_get_priority_max(SCHED_FIFO);
            // if (pthread_setschedparam(pthread_self(), SCHED_FIFO, &param)) {
            //     perror("Warning: Failed to set real-time priority");
            // }

            // 绑定core
            // cpu_set_t cpu_set;
            // CPU_ZERO(&cpu_set);
            // CPU_SET(3, &cpu_set);
            // pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu_set);
            // cpu_set_t cpu_get;
            // if (pthread_getaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu_get) != 0) {
            //     perror("Failed to get CPU affinity");
            //     return;
            // }

            struct pollfd pfds[2];
            pfds[0].fd = m_timer_fd;
            pfds[0].events = POLLIN;
            pfds[1].fd = m_event_fd;
            pfds[1].events = POLLIN;

            while (m_is_running.load()) {
                int ret = poll(pfds, 1, -1); // 阻塞等待事件
                if (ret < 0) {
                    if (errno == EINTR) {
                        continue; // 被信号中断，继续等待
                    }
                    perror("Failed to poll timerfd");
                    break;
                }
            
                // 处理定时事件
                if (pfds[0].revents & POLLIN) {
                    uint64_t expirations;
                    ssize_t bytes_read = read(m_timer_fd, &expirations, sizeof(expirations));
                    if (bytes_read != sizeof(expirations)) {
                        perror("Failed to read timerfd");
                        continue;
                    }

                    if (m_callback && expirations > 0) {
                        for (uint64_t i = 0; i < expirations; ++i) {
                            m_callback();
                        }
                    }
                }

            // 处理停止信号
            if (pfds[1].revents & POLLIN) {
                uint64_t dummy;
                read(m_event_fd, &dummy, sizeof(dummy));
                break;
            }
            }

            // 清理资源
            if (m_timer_fd != -1) {
                close(m_timer_fd);
                m_timer_fd = -1;
            }
            if (m_event_fd != -1) {
            close(m_event_fd);
            m_event_fd = -1;
        }
        });
        return true;
    }

    void stop() {
    if (!m_is_running.exchange(false)) return;

    // 发送停止信号
    uint64_t value = 1;
    if (write(m_event_fd, &value, sizeof(value)) == -1) {
        perror("write eventfd error");
    }

        if (m_thread.joinable()) {
            m_thread.join();
        }
    }

private:
    int m_timer_fd;
    int m_event_fd;
    uint64_t m_interval_ms;
    TaskCallback m_callback;
    std::thread m_thread;
    std::atomic<bool> m_is_running;
};

#endif // __TIMER_H__
**/