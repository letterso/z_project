---
applyTo: "**/CMakeLists.txt,**/*.cmake"
---

# CMake 编码规范

## 基本原则

- **禁止源内构建**：CMake 在检测到 `${CMAKE_SOURCE_DIR} == ${CMAKE_BINARY_DIR}` 时会报错退出
- 构建目录统一为 `build/`，通过 `make build` 触发
- 使用现代 CMake（目标导向），禁止使用 `include_directories()`、`link_libraries()` 等旧式命令

---

## 库目标命名

- 共享库目标名使用**全大写** + `_LIBS` 后缀
  ```cmake
  add_library(COMMON_LIBS SHARED ...)
  add_library(IPC_LIBS SHARED ...)
  ```

---

## 模块库 CMakeLists.txt 模板

```cmake
add_library(MODULE_LIBS SHARED
    src/foo.cpp
    src/bar.cpp)

target_include_directories(MODULE_LIBS PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

target_link_libraries(MODULE_LIBS PUBLIC
    COMMON_LIBS
    Threads::Threads)

set_target_properties(MODULE_LIBS PROPERTIES
    POSITION_INDEPENDENT_CODE ON)
```

对于需要**隐藏内部符号**的库（类似 ipc 模块），额外添加：

```cmake
set_target_properties(MODULE_LIBS PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    POSITION_INDEPENDENT_CODE ON)
```

---

## 应用程序 CMakeLists.txt 模板

```cmake
add_executable(my_node
    ./my_node.cpp)

target_link_libraries(my_node PUBLIC
    COMMON_LIBS)
```

若应用依赖 ipc 模块，链接对应目标：
```cmake
target_link_libraries(my_node PUBLIC
    IPC_LIBS
    COMMON_LIBS)
```

---

## Include 目录规则

- 始终使用生成器表达式区分构建与安装阶段：
  ```cmake
  target_include_directories(MY_LIB PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:include>)
  ```
- 禁止使用绝对路径硬编码 include 目录

---

## 预编译头（PCH）

`COMMON_LIBS` 已配置 PCH，新模块可复用：
```cmake
target_precompile_headers(MY_LIB REUSE_FROM COMMON_LIBS)
```

---

## 常用依赖目标名

| 依赖 | CMake 目标名 |
|------|-------------|
| spdlog | `3rdparty::spdlog` |
| yaml-cpp | `3rdparty::yaml-cpp` |
| POSIX 线程 | `Threads::Threads` |
| Eigen3 | `${EIGEN3_INCLUDE_DIR}`（include 目录） |
| doctest | `3rdparty::doctest` |

---

## 注册新子目录

在父级 `CMakeLists.txt` 中通过 `add_subdirectory` 注册：

```cmake
# modules/CMakeLists.txt
add_subdirectory(common)
add_subdirectory(ipc)
add_subdirectory(newmodule)   # 新增模块在此追加

# app/CMakeLists.txt
add_subdirectory(ipc_app)
add_subdirectory(timer_app)
add_subdirectory(new_app)     # 新增应用在此追加
```
