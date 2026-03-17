#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "common/safe_queue.hpp"

#include <atomic>
#include <string>
#include <thread>
#include <chrono>

TEST_SUITE("SafeQueue") {

  TEST_CASE("push存入元素后try_pop可获取") {
    SafeQueue<int> q(10);
    CHECK(q.push(42));
    int val = 0;
    CHECK(q.try_pop(val));
    CHECK(val == 42);
    CHECK(q.empty());
  }

  TEST_CASE("try_pop空队列返回false不修改输出值") {
    SafeQueue<int> q(10);
    int val = -1;
    CHECK_FALSE(q.try_pop(val));
    CHECK(val == -1);
  }

  TEST_CASE("size反映当前元素数量") {
    SafeQueue<int> q(10);
    q.push(1);
    q.push(2);
    q.push(3);
    CHECK(q.size() == 3);
  }

  TEST_CASE("push超容量返回false") {
    SafeQueue<int> q(2);
    CHECK(q.push(1));
    CHECK(q.push(2));
    CHECK_FALSE(q.push(3));
    CHECK(q.size() == 2);
  }

  TEST_CASE("stop后push返回false") {
    SafeQueue<int> q(10);
    q.stop();
    CHECK_FALSE(q.push(1));
  }

  TEST_CASE("push右值引用移动语义") {
    SafeQueue<std::string> q(10);
    std::string s = "hello";
    CHECK(q.push(std::move(s)));
    CHECK(s.empty());  // 源对象已被移走
    std::string val;
    CHECK(q.try_pop(val));
    CHECK(val == "hello");
  }

  TEST_CASE("emplace原地构造元素") {
    SafeQueue<std::string> q(10);
    CHECK(q.emplace(3, 'x'));
    std::string val;
    CHECK(q.try_pop(val));
    CHECK(val == "xxx");
  }

  TEST_CASE("pop阻塞直到生产者放入数据") {
    SafeQueue<int> q(10);
    std::thread producer([&q]() {
      std::this_thread::sleep_for(std::chrono::milliseconds(30));
      q.push(99);
    });
    int val = 0;
    const bool got = q.pop(val);
    producer.join();
    CHECK(got);
    CHECK(val == 99);
  }

  TEST_CASE("stop唤醒阻塞于pop的线程") {
    SafeQueue<int> q(10);
    std::thread stopper([&q]() {
      std::this_thread::sleep_for(std::chrono::milliseconds(30));
      q.stop();
    });
    int val = 0;
    const bool got = q.pop(val);
    stopper.join();
    CHECK_FALSE(got);
  }

  TEST_CASE("stop后try_pop仍可消费已有元素") {
    SafeQueue<int> q(10);
    q.push(1);
    q.push(2);
    q.stop();
    int val = 0;
    CHECK(q.try_pop(val));
    CHECK(val == 1);
    CHECK(q.try_pop(val));
    CHECK(val == 2);
    CHECK_FALSE(q.try_pop(val));
  }

  TEST_CASE("clear清空队列") {
    SafeQueue<int> q(10);
    q.push(1);
    q.push(2);
    q.clear();
    CHECK(q.empty());
  }

  TEST_CASE("多线程并发push和try_pop无数据竞争") {
    SafeQueue<int> q(1000);
    std::atomic<int> sum{0};
    constexpr int N = 500;

    std::thread producer([&q]() {
      for (int i = 0; i < N; ++i) {
        while (!q.push(i)) {
          std::this_thread::yield();
        }
      }
    });

    std::thread consumer([&q, &sum]() {
      int collected = 0;
      while (collected < N) {
        int val = 0;
        if (q.try_pop(val)) {
          sum.fetch_add(val);
          ++collected;
        } else {
          std::this_thread::yield();
        }
      }
    });

    producer.join();
    consumer.join();

    constexpr int expected = N * (N - 1) / 2;  // sum of 0..N-1
    CHECK(sum.load() == expected);
  }

}
