project(spdlog)

# Create INTERFACE library for header-only library
add_library(${PROJECT_NAME} INTERFACE)
add_library(3rdparty::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# Set properties for INTERFACE library
target_include_directories(
    ${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)