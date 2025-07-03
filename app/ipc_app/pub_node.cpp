#include "ipc/message_queue.h"
#include "common/thread_pool.h"
#include "common/logger.h"

#include <iostream>
#include <chrono>
#include <ctime>

#define M_TO_S_CHAN_KEY 99999999

int main(int argc, char **argv)
{
    // 日志初始化
    auto &logger_instance = Singleton<Logger>::instance();
    if (!logger_instance.init())
    {
        LOGC("Failed to create logger");
        return -1;
    } 

    MessageQueue _msg_chan;
    if (!_msg_chan.get_msg_queue(M_TO_S_CHAN_KEY))
    {
        LOGE("初始化接收队列失败");
        exit(-1);
    }

    ThreadPool thread_pool(4);
    MessageQueue::Message msg = {0};
    msg.type = 1;
    msg.text[0] = 123.456;
    msg.text[1] = 456.789;
    bool flag = _msg_chan.send(msg);
    LOGI("send time : {}", std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count() / 1e6);

    auto flag_future = thread_pool.submit([&]() -> bool
                                          { return _msg_chan.send(msg, false); });
    // flag_future.get();
    // _msg_chan.del_msg_queue();

    LOGI("send : {}", flag_future.get());

    return 0;
}