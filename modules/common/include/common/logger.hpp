#ifndef __LOGGER_H__
#define __LOGGER_H__

#include "singleton.hpp"
#include <spdlog/spdlog.h>

#include <memory>
#include <string>
#include <vector>

// normal日志
#define LOG_TOPIC "node"                            // 日志tag
#define LOG_FILE_SIZE 1024 * 1024 * 5               // 5MB
#define LOG_ROTATION 3                              // 日志文件满3个时开始滚动日志
#define LOG_FLUSH_F 1                               // 1秒flush一次
#define LOG_FLUSH_ON spdlog::level::info            // 当打印这个级别日志时flush
#define PATTERN "[%Y-%m-%d %H:%M:%S.%e][%^%L%$] %v" // 日志样式

// evaluate日志
#define LOG_TIME_TOPIC "time"
#define EVALOG_PATTERN "%v"                  // 日志样式

// 周期
#define LOG_MAX_DAY 30                        // 最大保存天数

class LoggerManager {
private:
    std::string _time_file_name;
    void create_directories();
    static void delete_directories();

public:
    LoggerManager();
    ~LoggerManager();
    std::string get_log_file_name() const;
};


#define LOGC(fmt, ...) \
    Singleton<Logger>::instance().log_critical(fmt, ##__VA_ARGS__)
#define LOGE(fmt, ...) \
    Singleton<Logger>::instance().log_error(fmt, ##__VA_ARGS__)
#define LOGW(fmt, ...) \
    Singleton<Logger>::instance().log_warn(fmt, ##__VA_ARGS__)
#define LOGI(fmt, ...) \
    Singleton<Logger>::instance().log_info(fmt, ##__VA_ARGS__)
#define LOGD(fmt, ...) \
    Singleton<Logger>::instance().log_debug(fmt, ##__VA_ARGS__)
#define LOGT(fmt, ...) \
    Singleton<Logger>::instance().log_trace(fmt, ##__VA_ARGS__)

#define EVALOGTIME(fmt, ...) \
    Singleton<Logger>::instance().log_time(fmt, ##__VA_ARGS__)

class Logger {
public:
    Logger();
    ~Logger();
    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;

    bool init();
    void flush() const;

    template <typename... Args>
    inline void log_critical(const char *fmt, Args... args){
        if (main_logger_) main_logger_->critical(fmt, args...); else SPDLOG_CRITICAL(fmt, args...);
    }

    template <typename... Args>
    inline void log_error(const char *fmt, Args... args) {
        if (main_logger_) main_logger_->error(fmt, args...); else SPDLOG_ERROR(fmt, args...);
    }

    template <typename... Args>
    inline void log_warn(const char *fmt, Args... args) {
        if (main_logger_) main_logger_->warn(fmt, args...); else SPDLOG_WARN(fmt, args...);
    }

    template <typename... Args>
    inline void log_info(const char *fmt, Args... args) {
        if (main_logger_) main_logger_->info(fmt, args...); else SPDLOG_INFO(fmt, args...);
    }

    template <typename... Args>
    inline void log_debug(const char *fmt, Args... args) {
        if (main_logger_) main_logger_->debug(fmt, args...); else SPDLOG_DEBUG(fmt, args...);
    }

    template <typename... Args>
    inline void log_trace(const char *fmt, Args... args) {
        if (main_logger_) main_logger_->trace(fmt, args...); else SPDLOG_TRACE(fmt, args...);
    }

    template <typename... Args>
    inline void log_time(const char *fmt, Args... args) {
        if (time_logger_) time_logger_->info(fmt, args...);
    }

private:
    std::vector<std::string> m_logger_name;
    std::shared_ptr<spdlog::logger> main_logger_;
    std::shared_ptr<spdlog::logger> time_logger_;

    static bool is_debug_mode();
    static bool is_evaluate_mode();
    void createAndConfigureLogger(const std::string &logger_name, const std::string &filename);
};

#endif  // __LOGGER_H__