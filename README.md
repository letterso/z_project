# 自用C++基础工程

构建通用c++开发工程框架，实现C++开发基础功能，包括日志，参数管理等。

索引

- [架构](#架构)
- [环境](#环境)
- [编译打包](#编译打包)
- [模块说明](#模块说明)
- [工具](#工具)

# 架构

```bash
.
├── 3rdparty        # 第三方库
├── app             # 应用层代码
├── cmake
├── CMakeLists.txt
├── config          # 参数    
├── README.md   
├── src             # 程序源码
├── tools           # python工具
└── xslt            # 一些用于生成 html 的模板
```

# 环境

- c++ 17
- yaml-cpp
- spdlog

```bash
sudo apt update
sudo apt install gcc g++ cmake make xsltproc cppcheck
bash .githook.sh # 配置git commit hook
```

# 编译打包

1. 编译第三方库 
    
    运行`build.sh`编译第三方库，自动替换第三方库的lib。

2. 源码编译

    ```bash
    make install    # 安装程序到project的install目录下
    make pack       # 打包程序为deb
    make test       # 编译代码和单元测试
    make build      # 编译代码
    make check      # 静态检测代码，输出结果在build目录cppcheck_report.xml/cppcheck_report.html
    make clean      # 删除编译缓存
    ```
    install和pack时要注意修改`APP_ROOT_DIR`路径，pack时该变量设置为打包安装路径

# 模块说明

## app

应用层，业务相关代码。

## src

### common

#### 参数

使用单例模式对yaml-cpp进行二次包装，实现全局的参数管理。

#### 日志

基于spdlog进行二次包装，日志目录架构，日志按照7天为一个周期，自动清除日期最旧的目录，当前日期下每次算法启动会根据当前时间创建目录

```
├── log
│   └── date
│       └── time
│           ├── node.log
│           └── time.log
```

启动程序前设置环境变量debug为on可以使程序进入debug模式，在该模式下会同时打印和记录debug日志，日志为滚动写入。

启动程序前设置环境变量evaluate为on可以使程序记录评估用日志，当前记录运行时间。

```bash
# debug
export debug=on     # 开启debug模式
export debug=off    # 关闭debug模式

# evaluate
export evaluate=on     # 开启evaluate模式
export evaluate=off    # 关闭evaluate模式
```

#### 定时器

基于timefd和epoll实现的高精度定时器。

#### 线程池

#### 耗时分析

- `TimeCostEva`：评估耗时并写入日志

- `TicToc`：手动指定启停的耗时分析

- `TimeCost`：raii机制实现的耗时评估

### ipc

基于消息队列实现

# 工具

- `plot_time.py ${time_log_file}`

  输出${time_log_file}的输出时间分析图。