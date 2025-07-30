find_package(Threads REQUIRED)

# common
add_library(COMMON_LIBS SHARED
        common/config.cpp
        common/logger.cpp
        common/timecost_utils.cpp)
target_include_directories(COMMON_LIBS PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(COMMON_LIBS PUBLIC
        3rdparty::yaml-cpp
        3rdparty::spdlog
        Threads::Threads)
set_target_properties(COMMON_LIBS PROPERTIES
        POSITION_INDEPENDENT_CODE ON)

target_precompile_headers(COMMON_LIBS PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/pch.h)

# ipc
add_library(IPC_LIBS SHARED
  ipc/message_queue.cpp)
target_link_libraries(IPC_LIBS PUBLIC
  Threads::Threads)
set_target_properties(IPC_LIBS PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        POSITION_INDEPENDENT_CODE ON)
