include_directories(${CMAKE_CURRENT_SOURCE_DIR})

find_package(Threads REQUIRED)
find_package(Eigen3 REQUIRED)

# common
add_library(COMMON_LIBS SHARED
        common/config.cpp
        common/timecost_utils.cpp)
target_include_directories(COMMON_LIBS PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR} 
        ${EIGEN3_INCLUDE_DIR})
target_link_libraries(COMMON_LIBS PUBLIC
        3rdparty::yaml-cpp
        3rdparty::spdlog
        Threads::Threads)

# ipc
add_library(IPC_LIBS SHARED
  ipc/message_queue.cpp)
target_link_libraries(IPC_LIBS PUBLIC
  Threads::Threads)