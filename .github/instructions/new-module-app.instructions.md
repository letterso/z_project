---
applyTo: "modules/**,app/**"
---

# 新增模块与应用规范

## 新增模块（`modules/`）

### 目录结构

```
modules/
└── mymodule/
    ├── CMakeLists.txt
    ├── include/
    │   └── mymodule/
    │       └── my_class.hpp
    └── src/
        └── my_class.cpp
```

### 步骤

1. 按上述结构创建目录和文件
2. 参考 [modules/ipc/CMakeLists.txt](../../modules/ipc/CMakeLists.txt) 编写 `CMakeLists.txt`
3. 在 [modules/CMakeLists.txt](../../modules/CMakeLists.txt) 末尾追加：
   ```cmake
   add_subdirectory(mymodule)
   ```

### 头文件模板

```cpp
#ifndef __MYMODULE_MY_CLASS_H__
#define __MYMODULE_MY_CLASS_H__

/**
 * @brief Brief description of MyClass.
 */
class MyClass {
 public:
  MyClass() = default;
  ~MyClass() = default;

  bool init();

 private:
  int _state{0};
};

#endif  // __MYMODULE_MY_CLASS_H__
```

### 若模块需要隐藏内部符号

头文件中对公开类添加可见性标注：
```cpp
class __attribute__((visibility("default"))) MyClass {
```
CMakeLists.txt 中添加：
```cmake
set_target_properties(MY_LIBS PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    POSITION_INDEPENDENT_CODE ON)
```

---

## 新增应用（`app/`）

### 目录结构

```
app/
└── myapp/
    ├── CMakeLists.txt
    └── my_node.cpp
```

### 步骤

1. 按上述结构创建目录和文件
2. 参考 [app/ipc_app/CMakeLists.txt](../../app/ipc_app/CMakeLists.txt) 编写 `CMakeLists.txt`
3. 在 [app/CMakeLists.txt](../../app/CMakeLists.txt) 末尾追加：
   ```cmake
   add_subdirectory(myapp)
   ```

### `my_node.cpp` 主函数模板

```cpp
#include "common/logger.hpp"
// 其他项目头文件...

#include <unistd.h>

void print_application_info() {
    LOGI(
        "\n"
        "App Name: {}    \n"
        "App Version: {} \n"
        "Build Date: {}  \n"
        "Git Hash:  {}   \n",
        APP_NAME, APP_VERSION, BUILD_DATE, GIT_HASH);
}

int main()
{
    // 1. 日志初始化（必须最先执行）
    auto& logger_instance = Singleton<Logger>::instance();
    if (!logger_instance.init())
    {
        LOGC("Failed to create logger");
        return -1;
    }

    // 2. 打印应用信息
    print_application_info();

    // 3. 初始化业务组件（Config、MessageQueue 等）

    // 4. 启动工作线程（ThreadPool 或独立线程）

    return 0;
}
```

### main 函数中的初始化顺序

必须严格遵守以下顺序：
1. **Logger 初始化**（`Singleton<Logger>::instance().init()`）
2. **Config 加载**（`Config::GetInstance().LoadFile("config/params.yaml")`）
3. **业务组件初始化**（MessageQueue、ThreadPool 等）
4. **启动工作线程**
