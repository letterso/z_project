#ifndef __LOGGER_H__
#define __LOGGER_H__

#include "make_unique.h"
#include "singleton.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#define ASYNC_MODE
#ifdef ASYNC_MODE
#include <spdlog/async.h>
#endif

#include <Eigen/Core>
#include <Eigen/Geometry>

#include <memory>
#include <filesystem>

// normal日志
#define LOG_DIR "./log"                             // 日志根目录
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
#define LOG_MAX_DAY 7                        // 最大保存天数

// Custom formatter for Eigen::DenseBase types
template<typename T>
struct fmt::formatter<T, std::enable_if_t<std::is_base_of<Eigen::DenseBase<T>, T>::value, char>> {
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        return ctx.end();
    }

    template <typename FormatContext>
    auto format(const Eigen::DenseBase<T>& mat, FormatContext& ctx) const -> decltype(ctx.out()) {
        std::stringstream ss;
        ss << mat;
        return fmt::format_to(ctx.out(), "{}", ss.str());
    }
};

class LoggerManager {
private:
    std::string _time_file_name;
    
private: 
    void create_directories() {
        // 获取当前日期
        auto current_time = std::chrono::system_clock::now();
        std::time_t time_t_format = std::chrono::system_clock::to_time_t(current_time);
        std::tm tm_format = *std::localtime(&time_t_format);
        
        // 使用年月日创建一级目录
        std::ostringstream date_oss;
        date_oss << LOG_DIR << "/" << std::put_time(&tm_format, "%Y%m%d");
        std::string date_file_name = date_oss.str();
        if (!std::filesystem::is_directory(date_file_name) || !std::filesystem::exists(date_file_name)) {
            std::filesystem::create_directories(date_file_name);
        }

        // 使用时分秒创建二级目录
        std::ostringstream time_oss;
        time_oss << LOG_DIR << "/" << std::put_time(&tm_format, "%Y%m%d") << "/"
                << std::put_time(&tm_format, "%H%M%S");
        _time_file_name = time_oss.str();
        if (!std::filesystem::is_directory(_time_file_name) || !std::filesystem::exists(_time_file_name)) {
            std::filesystem::create_directories(_time_file_name);
        }
    }

    static void delete_directories() {
        for (auto &entry : std::filesystem::directory_iterator(LOG_DIR)) { // 遍历当前文件夹
            if (entry.is_directory()) {
                // 获取文件夹名称
                std::string folder_name = entry.path().filename().string();

                // 确保文件夹名符合日期格式
                if (folder_name.size() == 8) {  // YYYYMMDD
                    std::tm folder_time = {};
                    std::istringstream ss(folder_name);
                    ss >> std::get_time(&folder_time, "%Y%m%d");

                    if (ss.fail()) {
                        SPDLOG_ERROR("[LOGGER] date parsing failed: {}", folder_name);
                        continue;
                    }

                    // 将文件夹时间转换为time_t
                    std::time_t folder_time_t = std::mktime(&folder_time);
                    std::time_t now = std::time(nullptr);

                    // 计算日期差
                    double days_old = std::difftime(now, folder_time_t) / (60 * 60 * 24);

                    // 删除超过最大保存天数的文件夹
                    if (days_old >= LOG_MAX_DAY) {
                        std::filesystem::remove_all(entry.path());
                        SPDLOG_INFO("[LOGGER] remove old directories: {}", entry.path().string());
                    }
                }
            }
        }
    }

public:
    LoggerManager() {
        create_directories();
        delete_directories();
    }

    std::string get_log_file_name() const{
        return _time_file_name;
    }

    ~LoggerManager() {}
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
    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;
    Logger() = default;
    ~Logger()
    {
        for (const auto &name : m_logger_name)
        {
            if (spdlog::get(name))
            {
                spdlog::get(name)->flush();
                spdlog::drop(name);
            }
        }
        spdlog::shutdown();  // 若工程中其他库使用spdlog或者和ROS2一起使用，需要注释
    }

    bool init() {
        auto &&function = [&]() {
#ifdef ASYNC_MODE
            // 初始化内存池
            spdlog::init_thread_pool(8192, 1);
            SPDLOG_INFO("[LOGGER] Use Async Mode");
#endif
            // 获取日志目录
            std::string log_file_name = Singleton<LoggerManager>::instance().get_log_file_name();

            // 创建主日志
            if(!m_logger){
                // 创建sinks
                auto stdout_sink = std::make_shared<stdout_sink_t>();
                auto rotating_sink = std::make_shared<rotating_sink_t>(log_file_name + "/" + LOG_TOPIC + ".log", LOG_FILE_SIZE, LOG_ROTATION);
                std::vector<spdlog::sink_ptr> sinks{stdout_sink, rotating_sink};
#ifdef ASYNC_MODE
                // 构建日志器
                m_logger = std::make_shared<spdlog::async_logger>(LOG_TOPIC, sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
#else
                m_logger = std::make_shared<spdlog::logger>(LOG_TOPIC, sinks.begin(), sinks.end());
#endif
                m_logger->set_pattern(PATTERN);
                if (is_debug_mode()) {
                    m_logger->set_level(spdlog::level::debug);
                } else {
                    m_logger->set_level(spdlog::level::info);
                }
                m_logger->flush_on(LOG_FLUSH_ON);
                m_logger_name.push_back(LOG_TOPIC);
                spdlog::register_logger(m_logger);
            }

            // 创建评估日志
            if (is_evaluate_mode()) {
                createAndConfigureLogger(LOG_TIME_TOPIC, log_file_name + "/" + LOG_TIME_TOPIC + ".log");
            } 
        };

        try {
            function();
            return true;
        } catch (const std::exception &e) {
            SPDLOG_ERROR("[LOGGER] Construct logger error: {}", e.what());
            return false;
        }

        // 设置全局定时刷新
        spdlog::flush_every(std::chrono::seconds(LOG_FLUSH_F));
    }

    inline void flush()
    {
        for (const auto &name : m_logger_name)
        {
            if (spdlog::get(name))
            {
                spdlog::get(name)->flush();
            }
        }
    }

    template <typename... Args>
    inline void log_critical(const char *fmt, Args... args)
    {
        if (m_logger)
        {
            m_logger->critical(fmt, args...);
        }
        else
        {
            SPDLOG_CRITICAL(fmt, args...);
        }
    }

    template <typename... Args>
    inline void log_error(const char *fmt, Args... args)
    {
        if (m_logger)
        {
            m_logger->error(fmt, args...);
        }
        else
        {
            SPDLOG_ERROR(fmt, args...);
        }
    }

    template <typename... Args>
    inline void log_warn(const char *fmt, Args... args)
    {
        if (m_logger)
        {
            m_logger->warn(fmt, args...);
        }
        else
        {
            SPDLOG_WARN(fmt, args...);
        }
    }

    template <typename... Args>
    inline void log_info(const char *fmt, Args... args)
    {
        if (m_logger)
        {
            m_logger->info(fmt, args...);
        }
        else
        {
            SPDLOG_INFO(fmt, args...);
        }
    }

    template <typename... Args>
    inline void log_debug(const char *fmt, Args... args)
    {
        if (m_logger)
        {
            m_logger->debug(fmt, args...);
        }
        else
        {
            SPDLOG_DEBUG(fmt, args...);
        }
    }

    template <typename... Args>
    inline void log_trace(const char *fmt, Args... args)
    {
        if (m_logger)
        {
            m_logger->trace(fmt, args...);
        }
        else
        {
            SPDLOG_TRACE(fmt, args...);
        }
    }

    template <typename... Args>
    inline void log_time(const char *fmt, Args... args)
    {
        if(spdlog::get(LOG_TIME_TOPIC))
            spdlog::get(LOG_TIME_TOPIC)->info(fmt, args...);
    }

private:
    std::shared_ptr<spdlog::logger> m_logger;
    std::vector<std::string> m_logger_name;

    using stdout_sink_t = spdlog::sinks::stdout_color_sink_mt;
    using rotating_sink_t = spdlog::sinks::rotating_file_sink_mt;
    using basic_sink_t = spdlog::sinks::basic_file_sink_mt;

    static bool is_debug_mode() {
        char *var = getenv("debug");
        if (nullptr == var) {
            return false;
        }
        if (0 == strcmp(var, "on")) {
            SPDLOG_INFO("[LOGGER] debug mode on");
            return true;
        }
        return false;
    }

    static bool is_evaluate_mode() {
        char *var = getenv("evaluate");
        if (nullptr == var) {
            return false;
        }
        if (0 == strcmp(var, "on")) {
            SPDLOG_INFO("[LOGGER] evaluate mode on");
            return true;
        }
        return false;
    }

    void createAndConfigureLogger(
        const std::string &logger_name,
        const std::string &filename)
    {
        // 创建文件 Sink
        auto file_sink = std::make_shared<basic_sink_t>(filename, true);

#ifdef ASYNC_MODE
        // 创建异步 Logger
        auto logger = std::make_shared<spdlog::async_logger>(
            logger_name,
            file_sink, // 每个Logger绑定自己的文件Sink
            spdlog::thread_pool(),
            spdlog::async_overflow_policy::block);
#else
        auto logger = std::make_shared<spdlog::logger>(logger_name, file_sink);
#endif

        // 应用相同的日志配置
        logger->set_pattern(EVALOG_PATTERN);
        logger->set_level(spdlog::level::info);
        logger->flush_on(spdlog::level::info);
        spdlog::register_logger(logger);
        m_logger_name.push_back(logger_name);
    }
};

#endif  // __LOGGER_H__