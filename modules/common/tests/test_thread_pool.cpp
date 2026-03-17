#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "common/thread_pool.hpp"

#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

TEST_SUITE("ThreadPool") {

  TEST_CASE("提交任务并通过future获取返回值") {
    ThreadPool pool(2);
    auto fut = pool.submit([]() { return 42; });
    CHECK(fut.get() == 42);
  }

  TEST_CASE("提交带参数的任务") {
    ThreadPool pool(2);
    auto fut = pool.submit([](int a, int b) { return a + b; }, 3, 4);
    CHECK(fut.get() == 7);
  }

  TEST_CASE("多任务并发执行全部完成") {
    ThreadPool pool(4);
    std::atomic<int> counter{0};
    std::vector<std::future<void>> futures;
    for (int i = 0; i < 20; ++i) {
      futures.push_back(pool.submit([&counter]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        counter.fetch_add(1);
      }));
    }
    for (auto& f : futures) {
      f.wait();
    }
    CHECK(counter.load() == 20);
  }

  TEST_CASE("pending_tasks在任务完成后归零") {
    ThreadPool pool(1);
    std::atomic<bool> block{true};

    // 单线程池：先占满worker
    auto fut_block = pool.submit([&block]() {
      while (block.load()) {
        std::this_thread::yield();
      }
    });

    // 追加两个任务进队列
    auto fut2 = pool.submit([]() {});
    auto fut3 = pool.submit([]() {});
    CHECK(pool.pending_tasks() >= 1);

    block.store(false);
    fut_block.wait();
    fut2.wait();
    fut3.wait();
    CHECK(pool.pending_tasks() == 0);
  }

  TEST_CASE("任务抛出异常不影响后续任务执行") {
    ThreadPool pool(1);
    auto fut1 = pool.submit([]() -> int {
      throw std::runtime_error("intentional error");
      return 0;
    });
    CHECK_THROWS_AS(fut1.get(), std::runtime_error);

    // worker 线程依然存活
    auto fut2 = pool.submit([]() { return 99; });
    CHECK(fut2.get() == 99);
  }

  TEST_CASE("析构时等待全部任务完成") {
    std::atomic<int> completed{0};
    {
      ThreadPool pool(4);
      for (int i = 0; i < 10; ++i) {
        pool.submit([&completed]() {
          std::this_thread::sleep_for(std::chrono::milliseconds(5));
          completed.fetch_add(1);
        });
      }
      // 析构时应等待所有任务完成
    }
    CHECK(completed.load() == 10);
  }

  TEST_CASE("有界队列：任务数量大于队列容量时提交阻塞直到有空位") {
    ThreadPool pool(1, 2);
    std::atomic<bool> block{true};

    // 占住唯一worker
    auto fut_block = pool.submit([&block]() {
      while (block.load()) {
        std::this_thread::yield();
      }
    });

    // 填满有界队列
    auto f1 = pool.submit([]() {});
    auto f2 = pool.submit([]() {});

    // 第三个提交在另一线程中，应该阻塞直到队列有空位
    std::atomic<bool> submitted{false};
    std::thread submitter([&pool, &submitted]() {
      auto fut = pool.submit([]() {});
      submitted.store(true);
      fut.wait();
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    // submitter 应仍在等待（或刚刚提交成功）
    block.store(false);
    fut_block.wait();
    f1.wait();
    f2.wait();
    submitter.join();
    CHECK(submitted.load());
  }

}
