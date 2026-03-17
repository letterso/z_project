---
applyTo: "**/*.cpp,**/*.hpp,**/*.h"
---

# C++ 编码规范

## 命名规范

### 类型命名
- 类名、结构体名：**PascalCase**（大驼峰）
  ```cpp
  class MessageQueue { ... };
  struct TimerConfig { ... };
  ```

### 函数与方法
- 普通函数、成员函数：**snake_case**
  ```cpp
  bool get_msg_queue(int key);
  void log_info(const std::string& msg);
  ```

### 成员变量
- 使用 **`_` 前缀** + snake_case（项目统一风格）
  ```cpp
  int _msgid;
  std::string _log_file_path;
  bool _is_running;
  ```

### 宏与常量
- 全大写 + 下划线分隔
  ```cpp
  #define LOG_FILE_SIZE 1024 * 1024 * 5
  #define M_TO_S_CHAN_KEY 99999999
  ```

### 文件名
- snake_case：`message_queue.hpp`、`thread_pool.cpp`

---

## 头文件保护

使用双下划线包围、全大写格式，末尾注释标注宏名：

```cpp
#ifndef __MESSAGE_QUEUE_H__
#define __MESSAGE_QUEUE_H__

// ... 内容 ...

#endif  // __MESSAGE_QUEUE_H__
```

对于带路径的头文件（如 `ipc/xxx.hpp`），使用：
```cpp
#ifndef __IPC_MESSAGE_QUEUE_H__
```

---

## Include 顺序

1. 项目头文件（使用 `""`，带模块前缀路径）
2. 系统 / POSIX 头文件（使用 `<>`）
3. C++ 标准库（使用 `<>`）

```cpp
#include "common/logger.hpp"
#include "common/thread_pool.hpp"
#include "ipc/message_queue.hpp"

#include <unistd.h>
#include <sys/msg.h>

#include <iostream>
#include <chrono>
#include <memory>
```

---

## 符号可见性（共享库）

为需要公开的类添加 `__attribute__((visibility("default")))`，配合 CMake 中的 `CXX_VISIBILITY_PRESET hidden`：

```cpp
class __attribute__((visibility("default"))) MessageQueue {
    // ...
};
```

---

## 注释规范

- **模块/接口**：使用英文 Doxygen 风格（`@brief`、`@param`、`@return`）
- **业务逻辑行内注释**：可使用中文
- **禁止**为显而易见的代码添加注释

```cpp
/**
 * @brief Initialize message queue with given key.
 * @param key System V IPC key
 * @return true on success
 */
bool get_msg_queue(int key);

// 初始化日志，必须在所有组件之前调用
auto& logger = Singleton<Logger>::instance();
```

---

## 标准版本

- 最低使用 **C++17**
- 优先使用 `std::optional`、结构化绑定、`if constexpr` 等现代特性
- 禁止裸 `new`/`delete`，使用智能指针（`std::unique_ptr`、`std::shared_ptr`）
