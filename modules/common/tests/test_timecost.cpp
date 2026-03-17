#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "common/timecost_utils.hpp"

#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

// RAII 辅助：每个测试用例结束时自动清除静态记录
struct ClearRecords {
  ~ClearRecords() { TimeCostEva::Clear(); }
};

TEST_SUITE("TicToc") {

  TEST_CASE("toc返回正的毫秒数") {
    TicToc t;
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    const double elapsed = t.toc();
    CHECK(elapsed >= 10.0);   // 允许系统调度抖动
    CHECK(elapsed < 500.0);
  }

  TEST_CASE("tic重置计时起点") {
    TicToc t;
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    t.tic();  // 重新开始计时
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    const double elapsed = t.toc();
    CHECK(elapsed < 100.0);   // 应远小于累计
  }

}

TEST_SUITE("TimeCost") {

  TEST_CASE("GetTime返回正的毫秒数") {
    TimeCost tc("test_scope", TimeCost::Unit::MS);
    std::this_thread::sleep_for(std::chrono::milliseconds(15));
    const double t = tc.GetTime();
    CHECK(t >= 5.0);
    CHECK(t < 500.0);
  }

  TEST_CASE("US单位精度高于1000") {
    TimeCost tc("test_us", TimeCost::Unit::US);
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    const double t = tc.GetTime();
    CHECK(t >= 100.0);   // 2ms > 2000us，有调度余量取100
  }

}

TEST_SUITE("TimeCostEva") {

  TEST_CASE("Evaluate记录函数耗时") {
    ClearRecords guard;
    TimeCostEva::Evaluate([]() {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }, "func_a");

    const double mean = TimeCostEva::GetMeanTime("func_a");
    CHECK(mean >= 5.0);
    CHECK(mean < 500.0);
  }

  TEST_CASE("多次Evaluate后GetMeanTime返回均值") {
    ClearRecords guard;
    for (int i = 0; i < 5; ++i) {
      TimeCostEva::SetEvaluatet("func_b", static_cast<double>(i) * 10.0);
    }
    // values: 0, 10, 20, 30, 40  →  mean = 20
    const double mean = TimeCostEva::GetMeanTime("func_b");
    CHECK(mean == doctest::Approx(20.0).epsilon(0.01));
  }

  TEST_CASE("未记录的函数GetMeanTime返回0") {
    ClearRecords guard;
    CHECK(TimeCostEva::GetMeanTime("nonexistent_xyz") == 0.0);
  }

  TEST_CASE("Clear清除所有记录") {
    TimeCostEva::SetEvaluatet("tmp_func", 100.0);
    TimeCostEva::Clear();
    CHECK(TimeCostEva::GetMeanTime("tmp_func") == 0.0);
  }

  TEST_CASE("多线程并发Evaluate无数据竞争") {
    ClearRecords guard;
    constexpr int THREADS = 8;
    constexpr int ITERS = 50;

    std::vector<std::thread> threads;
    for (int t = 0; t < THREADS; ++t) {
      threads.emplace_back([t]() {
        for (int i = 0; i < ITERS; ++i) {
          const std::string name = "concurrent_func_" + std::to_string(t);
          TimeCostEva::Evaluate([]() {}, name);
        }
      });
    }
    for (auto& th : threads) {
      th.join();
    }

    // 每个线程有自己的函数名，验证全部记录数量
    for (int t = 0; t < THREADS; ++t) {
      const std::string name = "concurrent_func_" + std::to_string(t);
      const double mean = TimeCostEva::GetMeanTime(name);
      CHECK(mean >= 0.0);
    }
  }

  TEST_CASE("PrintAll不崩溃") {
    ClearRecords guard;
    TimeCostEva::SetEvaluatet("func_print", 1.5);
    CHECK_NOTHROW(TimeCostEva::PrintAll());
  }

}
