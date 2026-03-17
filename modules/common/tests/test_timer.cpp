#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "common/timer.hpp"

#include <atomic>
#include <chrono>
#include <thread>

TEST_SUITE("Timer") {

  TEST_CASE("定时器以指定间隔触发回调") {
    std::atomic<int> count{0};
    Timer timer(20, [&count]() { count.fetch_add(1); });
    timer.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    timer.stop();

    // 20ms 间隔 × 150ms ≈ 7次，考虑调度抖动取宽松范围
    CHECK(count.load() >= 4);
    CHECK(count.load() <= 12);
  }

  TEST_CASE("stop后不再触发回调") {
    std::atomic<int> count{0};
    Timer timer(20, [&count]() { count.fetch_add(1); });
    timer.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(60));
    timer.stop();

    const int count_at_stop = count.load();
    std::this_thread::sleep_for(std::chrono::milliseconds(60));
    CHECK(count.load() == count_at_stop);
  }

  TEST_CASE("重复start是幂等的") {
    std::atomic<int> count{0};
    Timer timer(30, [&count]() { count.fetch_add(1); });
    timer.start();
    timer.start();  // 第二次调用应为no-op
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    timer.stop();
    CHECK(count.load() >= 1);
  }

  TEST_CASE("重复stop不崩溃") {
    std::atomic<int> count{0};
    Timer timer(30, [&count]() { count.fetch_add(1); });
    timer.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    timer.stop();
    CHECK_NOTHROW(timer.stop());  // 第二次stop应为no-op
  }

  TEST_CASE("析构前自动stop不崩溃") {
    std::atomic<int> count{0};
    {
      Timer timer(10, [&count]() { count.fetch_add(1); });
      timer.start();
      std::this_thread::sleep_for(std::chrono::milliseconds(30));
      // 析构时自动调用stop()
    }
    CHECK(count.load() >= 1);
  }

}
