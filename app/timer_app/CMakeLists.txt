add_executable(timer_node
   ./timer_node.cpp)
target_link_libraries(timer_node PUBLIC
   COMMON_LIBS
)
target_compile_definitions(timer_node PUBLIC
    APP_NAME="${APP_NAME}"
    APP_VERSION="${APP_VERSION}"
    BUILD_DATE="${BUILD_DATE}"
    GIT_HASH="${GIT_HASH}"
    ROOT_DIR="${APP_ROOT_DIR}"
)