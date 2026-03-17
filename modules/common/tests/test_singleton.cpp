#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "common/singleton.hpp"

// 用于测试的简单计数类，不继承 Singleton
struct Counter {
  explicit Counter(int init = 0) : value(init) {}
  void increment() { ++value; }
  int value;
};

// 用于验证带参数构造的类
struct Configurable {
  explicit Configurable(std::string name, int id)
      : name(std::move(name)), id(id) {}
  std::string name;
  int id;
};

TEST_SUITE("Singleton") {

  TEST_CASE("多次调用instance返回同一对象") {
    auto& a = Singleton<Counter>::instance();
    auto& b = Singleton<Counter>::instance();
    CHECK(&a == &b);
  }

  TEST_CASE("instance返回的是引用，修改后持久化") {
    auto& inst = Singleton<Counter>::instance();
    const int prev = inst.value;
    inst.increment();
    CHECK(Singleton<Counter>::instance().value == prev + 1);
  }

  TEST_CASE("不同类型的Singleton互相独立") {
    auto& c = Singleton<Counter>::instance();
    auto& f = Singleton<Configurable>::instance(std::string("test"), 42);
    CHECK(&c != reinterpret_cast<void*>(&f));
    CHECK(f.name == "test");
    CHECK(f.id == 42);
  }

  TEST_CASE("首次构造后忽略后续参数") {
    // 两次调用使用相同 Args 类型 (std::string, int)，命中同一 static 局部变量
    auto& inst = Singleton<Configurable>::instance(std::string("ignored"), 0);
    CHECK(inst.name == "test");
    CHECK(inst.id == 42);
  }

}
