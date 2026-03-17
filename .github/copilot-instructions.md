# Copilot Instructions for Z-Project

## Project Overview

C++17+ 基础工程框架，提供应用开发通用组件。模块化架构：核心组件在 `modules/`，可执行应用在 `app/`。

## 详细规范（专项 Instructions）

以下规范由独立 instructions 文件按文件类型自动加载，无需在此重复：

| 文件 | 覆盖范围 |
|------|---------|
| [instructions/cpp-style.instructions.md](instructions/cpp-style.instructions.md) | C++ 命名、头文件保护、include 顺序、可见性、注释风格 |
| [instructions/cmake.instructions.md](instructions/cmake.instructions.md) | CMake 库命名、目标模板、依赖链接、生成器表达式 |
| [instructions/new-module-app.instructions.md](instructions/new-module-app.instructions.md) | 新增模块/应用的目录结构、模板代码、初始化顺序 |
| [instructions/logging-config.instructions.md](instructions/logging-config.instructions.md) | 日志宏用法、Config 读取、Singleton 访问、ThreadPool |

---

## 架构速览

### 模块结构

```
modules/common/   → Logger, Config, ThreadPool, Singleton, Timer, FileManager
modules/ipc/      → MessageQueue（System V IPC）
app/ipc_app/      → pub_node, sub_node（消息队列收发示例）
app/timer_app/    → timer_node（定时任务示例）
3rdparty/         → spdlog, yaml-cpp, doctest（vendored）
config/           → params.yaml（运行时配置）
```

### 关键依赖

| 依赖 | 来源 | 用途 |
|------|------|------|
| spdlog | `3rdparty/spdlog/` | 日志后端 |
| yaml-cpp | `3rdparty/yaml-cpp/` | YAML 配置解析 |
| Eigen3 | 系统包管理器 | 线性代数 |
| doctest | `3rdparty/doctest/` | 单元测试 |

---

## 构建命令

```bash
make build      # 编译（Release 模式）
make test       # 编译 + 运行单元测试（-DENABLE_TEST=ON）
make check      # cppcheck 静态分析 → build/cppcheck_report.html
make install    # 安装到 install/
make pack       # 生成 .deb 包
make clean      # 删除 build/
```

- 构建目录：`build/`，输出：`build/bin/`（可执行文件）、`build/lib/`（库）
- **禁止源内构建**：`${CMAKE_SOURCE_DIR} == ${CMAKE_BINARY_DIR}` 时 CMake 报错退出

---

## 质量保障

- **Pre-commit hooks**：运行 `.githook.sh` 配置
- **静态分析**：仅检查 `src/` 和 `app/`，自动排除 ROS/Eigen3/PCL/TBB/Boost/OpenCV
- 配置：[cmake/modules/static_check.cmake](cmake/modules/static_check.cmake)

---

## 关键参考文件

| 文件 | 说明 |
|------|------|
| [CMakeLists.txt](CMakeLists.txt) | 顶层构建入口，禁止源内构建逻辑 |
| [makefile](makefile) | 所有工作流命令，`make help` 查看全部 |
| [cmake/debug.cmake](cmake/debug.cmake) | Debug 构建配置 |
| [cmake/pack.cmake](cmake/pack.cmake) | 打包配置 |
| [app/ipc_app/pub_node.cpp](app/ipc_app/pub_node.cpp) | 典型应用结构参考实现 |
