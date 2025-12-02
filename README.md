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

### 打包配置

DEB 包打包配置位于 `cmake/pack.cmake`，关键参数如下：

| 参数 | 说明 |
| :--- | :--- |
| `APP_VERSION` | 应用版本号，遵循 X.Y.Z 格式 |
| `PKG_NAME` | 包名，格式：`cn.z.${APP_NAME}` |
| `CPACK_PACKAGING_INSTALL_PREFIX` | 安装路径，默认：`$HOME/.Workspace/${PKG_NAME}` |
| `postinst` | DEB 安装后执行的脚本（如重载服务） |

**打包流程：**

```bash
make build      # 编译程序
make install    # 安装到 install/ 目录
make pack       # 生成 .deb 包
```

**查看 DEB 包信息：**

```bash
dpkg -I <package.deb>           # 查看包元数据
dpkg -c <package.deb>           # 查看包内容
dpkg -l | grep cn.z.zproject    # 检查是否已安装
```

**安装和卸载：**

```bash
sudo dpkg -i <package.deb>      # 安装
sudo dpkg -r cn.z.zproject      # 卸载
```

### 静态检查

使用 [Cppcheck](https://cppcheck.sourceforge.io/) 对项目C++代码进行静态分析，自动排除第三方库（ROS、Eigen3、PCL、Boost等）的检查。

- 使用

  ```bash
  cd build
  cmake ..
  
  # 方式1：生成完整报告（推荐）
  make all_cppcheck_reports
  
  # 方式2：只运行静态检查
  make run_cppcheck
  
  # 方式3：只生成HTML报告（需先运行cppcheck）
  make generate_cppcheck_html
  
  # 或者
  cmake --build . --target run_cppcheck --parallel           # 进行静态检查
  cmake --build . --target all_cppcheck_reports --parallel   # 进行静态检查并生成html报告
  ```

- 查看结果

  - **XML报告**：`build/cppcheck_report.xml`

  - **HTML报告**：`build/cppcheck_report.html` （推荐，可在浏览器中打开）

  - **过滤后的编译数据库**：`build/compile_commands_filtered.json`


- 配置说明

  静态检查配置位于 `cmake/modules/static_check.cmake`，已配置为：

  - ✅ 只检查 `src/` 和 `app/` 目录下的项目代码

  - ✅ 排除所有第三方库（ROS、Eigen3、PCL、TBB、Boost、OpenCV等）

  - ✅ 抑制配置相关的信息级别警告（noValidConfiguration、missingIncludeSystem）

  - ✅ 使用C++17标准进行检查
  
  如需修改检查范围或规则，编辑 `cmake/modules/static_check.cmake` 和 `cmake/modules/filter_compile_commands.py`。

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