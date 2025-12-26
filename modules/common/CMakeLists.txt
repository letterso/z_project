add_library(COMMON_LIBS SHARED
        src/config.cpp
        src/logger.cpp
        src/timecost_utils.cpp
        src/file_manager.cpp)

# 添加include目录
target_include_directories(COMMON_LIBS PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        ${EIGEN3_INCLUDE_DIR})

# 链接依赖库
target_link_libraries(COMMON_LIBS PUBLIC
        3rdparty::yaml-cpp
        3rdparty::spdlog
        Threads::Threads)
set_target_properties(COMMON_LIBS PROPERTIES
        POSITION_INDEPENDENT_CODE ON)

target_precompile_headers(COMMON_LIBS PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common/pch.h)
