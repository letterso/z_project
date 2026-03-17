#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "common/config.hpp"

#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

// 在临时目录写测试 YAML 文件
static std::string write_temp_yaml(const std::string& content) {
  const auto path = fs::temp_directory_path() / "z_project_test_config.yaml";
  std::ofstream ofs(path);
  ofs << content;
  return path.string();
}

TEST_SUITE("Config") {

  TEST_CASE("加载不存在的文件返回false") {
    CHECK_FALSE(Config::GetInstance().LoadFile("/tmp/z_project_nonexistent_9x8y7z.yaml"));
  }

  TEST_CASE("加载有效YAML文件返回true") {
    const auto path = write_temp_yaml(
        "section:\n"
        "  int_val: 42\n"
        "  float_val: 3.14\n"
        "  str_val: hello\n");
    CHECK(Config::GetInstance().LoadFile(path));
    fs::remove(path);
  }

  TEST_CASE("GetParam读取整型参数") {
    const auto path = write_temp_yaml(
        "section:\n"
        "  int_val: 42\n");
    Config::GetInstance().LoadFile(path);

    const auto val = Config::GetParam<int>("section", "int_val");
    CHECK(val.has_value());
    CHECK(val.value() == 42);
    fs::remove(path);
  }

  TEST_CASE("GetParam读取浮点数参数") {
    const auto path = write_temp_yaml(
        "section:\n"
        "  float_val: 3.14\n");
    Config::GetInstance().LoadFile(path);

    const auto val = Config::GetParam<double>("section", "float_val");
    CHECK(val.has_value());
    CHECK(val.value() == doctest::Approx(3.14).epsilon(0.001));
    fs::remove(path);
  }

  TEST_CASE("GetParam读取字符串参数") {
    const auto path = write_temp_yaml(
        "section:\n"
        "  name: world\n");
    Config::GetInstance().LoadFile(path);

    const auto val = Config::GetParam<std::string>("section", "name");
    CHECK(val.has_value());
    CHECK(val.value() == "world");
    fs::remove(path);
  }

  TEST_CASE("GetParam不存在的键返回nullopt") {
    const auto path = write_temp_yaml("section:\n  key: 1\n");
    Config::GetInstance().LoadFile(path);

    const auto val = Config::GetParam<int>("section", "missing_key_xyz");
    CHECK_FALSE(val.has_value());
    fs::remove(path);
  }

  TEST_CASE("GetParam带默认值：键缺失时返回默认值") {
    const auto path = write_temp_yaml("section:\n  key: 10\n");
    Config::GetInstance().LoadFile(path);

    const int v = Config::GetParam<int>("section", "no_such_key", 99);
    CHECK(v == 99);
    fs::remove(path);
  }

  TEST_CASE("GetParam带默认值：键存在时返回实际值") {
    const auto path = write_temp_yaml("section:\n  key: 10\n");
    Config::GetInstance().LoadFile(path);

    const int v = Config::GetParam<int>("section", "key", 99);
    CHECK(v == 10);
    fs::remove(path);
  }

}
