---
applyTo: "**/*.cpp,**/*.hpp,**/*.h"
---

# 日志与配置使用规范

## 日志系统（Logger + spdlog）

### 初始化

在 `main()` 最开始，必须先初始化 Logger：

```cpp
auto& logger_instance = Singleton<Logger>::instance();
if (!logger_instance.init())
{
    LOGC("Failed to create logger");
    return -1;
}
```

### 日志宏一览

| 宏 | 级别 | 使用场景 |
|----|------|---------|
| `LOGC(fmt, ...)` | Critical | 致命错误，程序无法继续 |
| `LOGE(fmt, ...)` | Error | 可恢复的错误 |
| `LOGW(fmt, ...)` | Warn | 警告，潜在问题 |
| `LOGI(fmt, ...)` | Info | 正常运行信息 |
| `LOGD(fmt, ...)` | Debug | 调试信息 |
| `LOGT(fmt, ...)` | Trace | 详细追踪 |
| `EVALOGTIME(fmt, ...)` | 时间评估 | 记录耗时数据到独立时间日志 |

### 格式化语法（spdlog / fmtlib 风格）

```cpp
LOGI("接收到消息，类型: {}, 值: {:.3f}", msg.type, msg.data);
LOGE("初始化失败，错误码: {}", errno);
LOGD("当前 CPU: {}, 时间戳: {} ms", sched_getcpu(), timestamp);
```

### 禁止事项

- 禁止在热路径（高频回调）中使用 `LOGD`/`LOGT` 以外的级别（避免性能影响）
- 禁止使用 `std::cout`/`std::cerr` 替代日志宏
- 禁止在 Logger 初始化前调用任何日志宏

---

## 配置系统（Config + yaml-cpp）

### 加载配置文件

```cpp
Config::GetInstance().LoadFile("config/params.yaml");
```

### 读取参数

`GetParam<T>` 返回 `std::optional<T>`，**必须**处理无值情况：

```cpp
// 推荐写法：提供默认值
auto rate = Config::GetParam<int>("timer", "rate").value_or(100);

// 有效性检查
auto host = Config::GetParam<std::string>("server", "host");
if (!host.has_value()) {
    LOGE("配置项 server.host 缺失");
    return -1;
}
```

### params.yaml 结构约定

```yaml
# 按模块分组，使用父键/子键两层结构
timer:
  rate: 100          # Hz
  max_delay_ms: 50

server:
  host: "localhost"
  port: 8080
```

---

## Singleton 访问模式

```cpp
// 获取单例引用（推荐）
auto& instance = Singleton<MyClass>::instance();

// 若构造函数需要参数
auto& instance = Singleton<MyClass>::instance(arg1, arg2);
```

- **禁止**手动创建 Logger、Config 等单例类的实例
- 单例仅在首次调用 `instance()` 时构造，之后返回同一对象

---

## ThreadPool 使用

```cpp
// 创建线程池
ThreadPool pool(4);               // 4 个工作线程
ThreadPool pool(4, 100);          // 4 个线程，最大队列 100

// 提交任务（返回 std::future）
auto future = pool.submit([&]() -> bool {
    return do_work();
});

// 等待结果
bool result = future.get();
```

- 线程池析构时**自动 join** 所有工作线程，无需手动停止
- 避免在任务中捕获临时变量的引用（生命周期问题）
