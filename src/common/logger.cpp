#include "logger.h"
#include "singleton.h"
#include "file_manager.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#define ASYNC_MODE
#ifdef ASYNC_MODE
#include <spdlog/async.h>
#endif

#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>

// --- LoggerManager Implementation ---

void LoggerManager::create_directories() {
    // Get the log directory from FileManager
    auto log_dir_opt = FileManager::GetInstance().GetLogDirectory();
    if (!log_dir_opt.has_value()) {
        spdlog::error("[LOGGER] Failed to get log directory from FileManager");
        return;
    }
    
    std::string log_dir_str = log_dir_opt.value().string();
    
    // 获取当前日期
    auto current_time = std::chrono::system_clock::now();
    std::time_t time_t_format = std::chrono::system_clock::to_time_t(current_time);
    std::tm tm_format = *std::localtime(&time_t_format);

    // 使用年月日创建一级目录
    std::ostringstream date_oss;
    date_oss << log_dir_str << "/" << std::put_time(&tm_format, "%Y%m%d");
    std::string date_file_name = date_oss.str();
    if (!std::filesystem::is_directory(date_file_name) || !std::filesystem::exists(date_file_name)) {
        std::filesystem::create_directories(date_file_name);
    }

    // 使用时分秒创建二级目录
    std::ostringstream time_oss;
    time_oss << date_file_name << "/" << std::put_time(&tm_format, "%H%M%S");
    _time_file_name = time_oss.str();
    if (!std::filesystem::is_directory(_time_file_name) || !std::filesystem::exists(_time_file_name)) {
        std::filesystem::create_directories(_time_file_name);
    }
}

void LoggerManager::delete_directories() {
    // Get the log directory from FileManager
    auto log_dir_opt = FileManager::GetInstance().GetLogDirectory();
    if (!log_dir_opt.has_value()) {
        spdlog::error("[LOGGER] Failed to get log directory from FileManager");
        return;
    }
    
    try {
        for (auto &entry : std::filesystem::directory_iterator(log_dir_opt.value())) {
            if (entry.is_directory()) {
                // 获取文件夹名称
                std::string folder_name = entry.path().filename().string();

                // 确保文件夹名符合日期格式
                if (folder_name.size() == 8) { // YYYYMMDD
                    std::tm folder_time = {};
                    std::istringstream ss(folder_name);
                    ss >> std::get_time(&folder_time, "%Y%m%d");

                    if (ss.fail()) {
                        spdlog::error("[LOGGER] date parsing failed: {}", folder_name);
                        continue;
                    }

                    // 将文件夹时间转换为time_t并计算日期差
                    std::time_t folder_time_t = std::mktime(&folder_time);
                    std::time_t now = std::time(nullptr);
                    double days_old = std::difftime(now, folder_time_t) / (60 * 60 * 24);

                    // 删除超过最大保存天数的文件夹
                    if (days_old >= LOG_MAX_DAY) {
                        std::filesystem::remove_all(entry.path());
                        spdlog::info("[LOGGER] remove old directories: {}", entry.path().string());
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        spdlog::error("[LOGGER] Error deleting old directories: {}", e.what());
    }
}

LoggerManager::LoggerManager() {
    create_directories();
    delete_directories();
}

LoggerManager::~LoggerManager() {}

std::string LoggerManager::get_log_file_name() const {
    return _time_file_name;
}

// --- Logger Implementation ---

Logger::Logger() = default;

Logger::~Logger() {
    for (const auto &name : m_logger_name) {
        if (spdlog::get(name)) {
            spdlog::get(name)->flush();
            spdlog::drop(name);
        }
    }
    // spdlog::shutdown();  // 若工程中其他库使用spdlog或者和ROS2一起使用，需要注释
}

bool Logger::init() {
    try {
        #ifdef ASYNC_MODE
        spdlog::init_thread_pool(8192, 1);
        SPDLOG_INFO("[LOGGER] Use Async Mode");
        #endif

        // 获取日志目录
        std::string log_file_name = Singleton<LoggerManager>::instance().get_log_file_name();

        // 创建主日志
        if (!spdlog::get(LOG_TOPIC)) {
            auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(log_file_name + "/" + LOG_TOPIC + ".log", LOG_FILE_SIZE, LOG_ROTATION);
            std::vector<spdlog::sink_ptr> sinks{stdout_sink, rotating_sink};

            #ifdef ASYNC_MODE
            auto logger = std::make_shared<spdlog::async_logger>(LOG_TOPIC, sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
            #else
            auto logger = std::make_shared<spdlog::logger>(LOG_TOPIC, sinks.begin(), sinks.end());
            #endif

            logger->set_pattern(PATTERN);
            logger->set_level(is_debug_mode() ? spdlog::level::debug : spdlog::level::info);
            logger->flush_on(LOG_FLUSH_ON);
            m_logger_name.push_back(LOG_TOPIC);
            spdlog::register_logger(logger);
        }

        // 创建评估日志
        if (is_evaluate_mode()) {
            createAndConfigureLogger(LOG_TIME_TOPIC, log_file_name + "/" + LOG_TIME_TOPIC + ".log");
        }

        spdlog::flush_every(std::chrono::seconds(LOG_FLUSH_F));
        return true;
    } catch (const std::exception &e) {
        SPDLOG_ERROR("[LOGGER] Construct logger error: {}", e.what());
        return false;
    }
}

void Logger::flush() const {
    for (const auto &name : m_logger_name) {
        if (spdlog::get(name)) {
            spdlog::get(name)->flush();
        }
    }
}

bool Logger::is_debug_mode() {
    if (const char *var = std::getenv("debug")) {
        if (std::string(var) == "on") {
            SPDLOG_INFO("[LOGGER] debug mode on");
            return true;
        }
    }
    return false;
}

bool Logger::is_evaluate_mode() {
    if (const char *var = std::getenv("evaluate")) {
        if (std::string(var) == "on") {
            SPDLOG_INFO("[LOGGER] evaluate mode on");
            return true;
        }
    }
    return false;
}

void Logger::createAndConfigureLogger(const std::string &logger_name, const std::string &filename) {
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename, true);

    #ifdef ASYNC_MODE
    auto logger = std::make_shared<spdlog::async_logger>(logger_name, file_sink, spdlog::thread_pool(), spdlog::async_overflow_policy::block);
    #else
    auto logger = std::make_shared<spdlog::logger>(logger_name, file_sink);
    #endif

    logger->set_pattern(EVALOG_PATTERN);
    logger->set_level(spdlog::level::info);
    logger->flush_on(spdlog::level::info);
    spdlog::register_logger(logger);
    m_logger_name.push_back(logger_name);
}
