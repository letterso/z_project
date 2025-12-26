#include "common/timer.hpp"
#include "common/logger.hpp"
#include "common/file_manager.hpp"

#include <unistd.h>
#include <iostream>
#include <chrono>

// figlet -f slant 生成 
void print_application_info() {
    LOGI(
        "\n"
        "   __  _                               \n"
        "  / /_(_)___ ___  ___  _____           \n"
        " / __/ / __ `__ \\/ _ \\/ ___/         \n"
        "/ /_/ / / / / / /  __/ /               \n" 
        "\\__/_/_/ /_/ /_/\\___/_/              \n"
        "+++++++++++++++++++++++++++++++++++    \n"
        "App Name: {}                           \n"
        "App Version: {}                        \n"
        "Build Date: {}                         \n"
        "Git Hash:  {}                          \n"
        "+++++++++++++++++++++++++++++++++++    \n",
        APP_NAME, APP_VERSION, BUILD_DATE, GIT_HASH);
}

int main()
{
    // 日志初始化
    auto &logger_instance = Singleton<Logger>::instance();
    if (!logger_instance.init())
    {
        LOGC("Failed to create logger");
        return -1;
    }    

    // 节点信息
    print_application_info();

    // 运行信息
    auto exe_path = FileManager::GetInstance().GetExecutablePath();
    if(exe_path.has_value())
        LOGI("Executable Path: {}", exe_path.value().string());

    double last_time = 0;
    Timer timer(100, [&]
                {
        auto now = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::microseconds>(
            now.time_since_epoch()
        );
        double cur_time = ms.count() / 1000.0;
        LOGI("on cpu : {}, since epoch: {}", sched_getcpu() , cur_time - last_time);
        EVALOGTIME("{}", cur_time - last_time);
        last_time = cur_time; });
    timer.start();
    sleep(20);
    timer.stop();
    return 0;
}