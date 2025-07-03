#include "ipc/message_queue.h"
#include "common/logger.h"

#include <chrono>
#include <thread>
#include <iostream>
#include <execinfo.h>
#include <signal.h>
#include <cstdlib>

#define M_TO_S_CHAN_KEY 99999999

bool run_flag = true;
void sig_handler(int sig)
{
  LOGE("signal: {}", sig);
  void *array[10];
  // 获取堆栈内容，Debug模式下可用
  auto size = backtrace(array, 10);
  char **strings = backtrace_symbols(array, size);
  for (auto i = 0; i < size; i++)
  {
    LOGE("Backtrace: {}", strings[i]); // 写堆栈日志
  }
  free(strings);
  // 重新发出信号以触发默认崩溃行为
  signal(sig, SIG_DFL);
  raise(sig);
}

int main(int argc, char **argv)
{
  signal(SIGTERM, sig_handler); // 发送给程序的终止请求
  signal(SIGSEGV, sig_handler); // 非法内存访问（分段错误）11
  signal(SIGPIPE, sig_handler); // PIPE破裂
  // signal(SIGINT, sig_handler);   // 外部中断，通常为用户所起始
  signal(SIGILL, sig_handler);  // 非法程序映像，例如非法指令
  signal(SIGABRT, sig_handler); // 异常终止条件，例如为 std::abort() 所起始
  signal(SIGFPE, sig_handler);  // 错误算术运算，例如除以零
  
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

  MessageQueue::Message msg = {0};
  while (run_flag)
  {
    if (_msg_chan.recv(msg))
    {
      LOGI("recv time: {}", std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count() / 1e6);
      LOGI("recv: [{}, {}]", msg.text[0], msg.text[1]);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(5)); // 200hz
  }
  _msg_chan.del_msg_queue();

  return 0;
}