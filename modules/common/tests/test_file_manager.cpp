#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "common/file_manager.hpp"

#include <filesystem>

namespace fs = std::filesystem;

TEST_SUITE("FileManager") {

  TEST_CASE("GetInstance返回同一单例") {
    auto& a = FileManager::GetInstance();
    auto& b = FileManager::GetInstance();
    CHECK(&a == &b);
  }

  TEST_CASE("GetExecutablePath返回有效可执行文件路径") {
    const auto path = FileManager::GetInstance().GetExecutablePath();
    CHECK(path.has_value());
    CHECK(fs::exists(path.value()));
    CHECK(path.value().is_absolute());
  }

  TEST_CASE("GetDirectory返回父目录") {
    const auto& mgr = FileManager::GetInstance();
    const fs::path test_path = "/home/user/config/params.yaml";
    const auto dir = mgr.GetDirectory(test_path);
    CHECK(dir == fs::path("/home/user/config"));
  }

  TEST_CASE("GetConfigDirectory返回以config结尾的路径") {
    const auto dir = FileManager::GetInstance().GetConfigDirectory();
    if (dir.has_value()) {
      CHECK(dir.value().filename() == "config");
    }
    // 若可执行文件路径无法确定则跳过该断言
  }

  TEST_CASE("GetLogDirectory返回以log结尾的路径") {
    const auto dir = FileManager::GetInstance().GetLogDirectory();
    if (dir.has_value()) {
      CHECK(dir.value().filename() == "log");
    }
  }

}
