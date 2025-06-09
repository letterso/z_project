#ifndef __LOGGER_H__
#define __LOGGER_H__

#include "make_unique.h"
#include "singleton.h"

#include <spdlog/async.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <thread>
#include <filesystem>

// normal日志
#define LOG_DIR "./log"                               // 日志根目录
#define LOG_TOPIC "node"                              // 日志tag
#define LOG_FILE_SIZE 1024 * 1024 * 5                 // 5MB
#define LOG_ROTATION 3                                // 日志文件满3个时开始滚动日志
#define LOG_FLUSH_F 1                                 // 1秒flush一次
#define LOG_FLUSH_ON spdlog::level::warn              // 当打印这个级别日志时flush
#define PATTERN "[%Y-%m-%d %H:%M:%S.%f] [%^%L%$] %v"  // 日志样式

// evaluate日志
#define LOG_TIME_TOPIC "time"
#define EVALOG_FLUSH_ON spdlog::level::info  // 当打印这个级别日志时flush
#define EVALOG_PATTERN "%v"                  // 日志样式

// 周期
#define LOG_MAX_DAY 7                        // 最大保存天数

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

    void delete_directories() {
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
                        std::cerr << "[LOGGER] date parsing failed: " << folder_name << std::endl;
                        continue;
                    }

                    // 将文件夹时间转换为时间_t
                    std::time_t folder_time_t = std::mktime(&folder_time);
                    std::time_t now = std::time(nullptr);

                    // 计算日期差
                    double days_old = std::difftime(now, folder_time_t) / (60 * 60 * 24);

                    // 删除超过最大保存天数的文件夹
                    if (days_old >= LOG_MAX_DAY) {
                        std::filesystem::remove_all(entry.path());
                        std::cout << "[LOGGER] remove old directories: " << entry.path() << std::endl;
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


#define LOGE(...) Singleton<Logger>::instance().log_error(__VA_ARGS__)
#define LOGW(...) Singleton<Logger>::instance().log_warn(__VA_ARGS__)
#define LOGI(...) Singleton<Logger>::instance().log_info(__VA_ARGS__)
#define LOGD(...) Singleton<Logger>::instance().log_debug(__VA_ARGS__)
#define LOGC(...) Singleton<Logger>::instance().log_critical(__VA_ARGS__)
class Logger {
private:
    std::unique_ptr<spdlog::logger> _logger;

private:
    static bool is_debug_mode() {
        char *var = getenv("debug");
        if (nullptr == var) {
            return false;
        }
        if (0 == strcmp(var, "on")) {
            std::cout << "[LOGGER] debug mode "<< std::endl;
            return true;
        }
        return false;
    }

public:
    Logger() {
        auto function = [&]() {
            std::string log_file_name = Singleton<LoggerManager>::instance().get_log_file_name();
            auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                log_file_name + "/" + LOG_TOPIC + ".log", LOG_FILE_SIZE, LOG_ROTATION);
            std::vector<spdlog::sink_ptr> sinks{stdout_sink, rotating_sink};
            _logger = std::make_unique<spdlog::logger>(LOG_TOPIC, sinks.begin(), sinks.end());
            _logger->set_pattern(PATTERN);
            if (is_debug_mode()) {
                _logger->set_level(spdlog::level::debug);
            } else {
                _logger->set_level(spdlog::level::info);
            }
            _logger->flush_on(LOG_FLUSH_ON);
        };
        try {
            function();
        } catch (const std::exception &e) {
            std::cout << "[LOGGER] Construct logger error: " << e.what() << std::endl;
        }
        std::thread([&]() {
            while (true) {
                _logger->flush();
                std::this_thread::sleep_for(std::chrono::seconds(LOG_FLUSH_F));
            }
        }).detach();
    }
    
    ~Logger() { 
        _logger->flush();
        spdlog::drop(LOG_TOPIC);
    }

    template <typename... Args>
    inline void log_error(const char *fmt, Args... args) {
        _logger->error(fmt, args...);
    }

    template <typename... Args>
    inline void log_warn(const char *fmt, Args... args) {
        _logger->warn(fmt, args...);
    }

    template <typename... Args>
    inline void log_info(const char *fmt, Args... args) {
        _logger->info(fmt, args...);
    }

    template <typename... Args>
    inline void log_debug(const char *fmt, Args... args) {
        _logger->debug(fmt, args...);
    }

    template <typename... Args>
    inline void log_critical(const char *fmt, Args... args) {
        _logger->critical(fmt, args...);
    }
};

#define EVALOGTIME(...) Singleton<EvaluateLogger>::instance().log_time(__VA_ARGS__)
class EvaluateLogger {
   private:
    std::shared_ptr<spdlog::logger> _time_logger;
    bool _evaluate_flag;

   private:
    bool is_evaluate_mode() {
        char *var = getenv("evaluate");
        if (nullptr == var) {
            _evaluate_flag = false;
            return false;
        }
        if (0 == strcmp(var, "on")) {
            _evaluate_flag = true;
            return true;
        }
        return false;
    }

   public:
    EvaluateLogger() {
        auto function = [&]() {
            if (is_evaluate_mode()) {
                std::string log_file_name = Singleton<LoggerManager>::instance().get_log_file_name();
                _time_logger = spdlog::basic_logger_mt<spdlog::async_factory>(
                    LOG_TIME_TOPIC, log_file_name + "/" + LOG_TIME_TOPIC + ".log", true);
                _time_logger->set_pattern(EVALOG_PATTERN);
                _time_logger->set_level(spdlog::level::info);
                _time_logger->flush_on(EVALOG_FLUSH_ON);
            } 
        };
        try {
            function();
        } catch (const std::exception &e) {
            std::cout << "[LOGGER] Construct logger error: " << e.what() << std::endl;
        }
    }

    ~EvaluateLogger() { 
       if (is_evaluate_mode()) {
           _time_logger->flush();
           spdlog::drop(LOG_TIME_TOPIC);
       }
    }

    template <typename... Args>
    inline void log_time(const char *fmt, Args... args) {
        if(_evaluate_flag)
            _time_logger->info(fmt, args...);
    }
};

#endif  // __LOGGER_H__