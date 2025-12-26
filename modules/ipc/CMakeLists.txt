add_library(IPC_LIBS SHARED
        src/message_queue.cpp)

# 添加include目录
target_include_directories(IPC_LIBS PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

# 链接依赖库
target_link_libraries(IPC_LIBS PUBLIC
        Threads::Threads)

# 设置目标属性
set_target_properties(IPC_LIBS PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        POSITION_INDEPENDENT_CODE ON)