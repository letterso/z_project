#include "common/timer.h"
#include "common/logger.h"

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
    print_application_info();

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