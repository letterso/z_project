# C++ 基础工程框架

本项目是一个通用的 C++ 工程框架，旨在为快速开发应用程序提供一个坚实的基础。它内置了日志管理、参数配置、IPC通信、线程池等常用模块。

## 目录结构

```
.
├── 3rdparty        # 第三方库 (例如 spdlog, yaml-cpp)
├── app             # 应用层代码，存放具体的可执行程序
├── cmake           # CMake 辅助模块
├── config          # 配置文件目录
├── src             # 核心源码 (可被多个应用共享的库)
├── tools           # 实用工具脚本 (例如 python)
├── xslt            # XSLT 模板 (例如用于代码检查报告)
├── .githook.sh     # Git 钩子配置脚本
├── CMakeLists.txt  # 主 CMake 配置文件
└── makefile        # 工程管理 Makefile
```

## 快速开始

### 1. 环境准备

在开始之前，请确保您已安装以下依赖：

- C++17 编译器 (GCC/G++)
- CMake
- `xsltproc` (用于生成静态检查报告)
- `cppcheck` (静态代码分析工具)

您可以使用以下命令在基于 Debian/Ubuntu 的系统上安装所有必需的工具：

```bash
sudo apt update
sudo apt install -y gcc g++ cmake xsltproc cppcheck
```

### 2. 配置 Git 钩子

为了保证代码提交质量，建议运行以下脚本来配置 Git commit 钩子：

```bash
bash .githook.sh
```

## 编译开发

| 命令           | 描述                                                         |
| :------------- | :----------------------------------------------------------- |
| `make build`   | 编译所有源代码，生成库和可执行文件。            |
| `make test`    | 编译并运行所有单元测试。                                     |
| `make check`   | 对代码进行静态分析，并在 `build` 目录下生成 `cppcheck_report.html` 报告。 |
| `make install` | 将程序安装到工程根目录下的 `install/` 目录中。               |
| `make pack`    | 将 `install/` 目录的内容打包成一个 Debian (`.deb`) 安装包。  |
| `make clean`   | 删除 `build/` 目录，清除所有编译缓存。                       |
| `make help`    | 显示所有可用的 `make` 命令及其描述。                         |

install和pack时要注意修改`APP_ROOT_DIR`路径，pack时该变量设置为打包安装路径

## 功能模块说明

### `src` - 核心库

`src` 目录包含了工程的核心功能，被编译成共享库供 `app` 中的应用使用。

- **`common`**: 通用基础模块。
  - **参数管理 (`config`)**: 基于 `yaml-cpp` 的全局单例参数管理器。
  - **日志 (`logger`)**: 基于 `spdlog` 的高性能日志系统，支持按日期和时间分目录、自动清理旧日志。
  - **定时器 (`timer`)**: 基于 `timerfd` 和 `epoll` 的高精度定时器。
  - **线程池 (`thread_pool`)**: 用于管理和复用线程的实用工具。
  - **耗时分析 (`timecost_utils`)**: 提供多种工具，用于精确测量和记录代码块的执行时间。
- **`ipc`**: 进程间通信模块，当前基于 System V 消息队列实现。
- **`interface`**: 定义通用接口。

### `app` - 应用层

`app` 目录存放使用 `src` 库构建的具体应用案例和可执行文件。

## 运行时配置

### 日志模式

通过设置环境变量，可以控制日志的输出级别和内容。

- **调试模式**: 输出详细的 `DEBUG` 级别日志。

  ```bash
  export debug=on # 默认关闭
  ```

- **评估模式**: 开启专用的评估日志（例如，用于记录性能数据）。

  ```bash
  export evaluate=on # 默认关闭
  ```

- **日志目录架构**

    ```
    ├── log
    │   └── date
    │       └── time
    │           ├── node.log
    │           └── time.log
    ```

## 工具

- **`tools/plot_time.py`**:
  一个 Python 脚本，用于可视化由耗时分析模块生成的日志文件。

  ```bash
  python3 tools/plot_time.py <path_to_time_log_file>
  ```