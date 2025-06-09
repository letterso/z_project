project(yaml-cpp)

add_library(
    ${PROJECT_NAME} STATIC IMPORTED
    GLOBAL
)
add_library(
    3rdparty::${PROJECT_NAME} ALIAS
    ${PROJECT_NAME}
)
set_target_properties(
    ${PROJECT_NAME} PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/libyaml-cpp.so.0.8
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/include
)
