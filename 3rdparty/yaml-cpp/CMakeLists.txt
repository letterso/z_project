project(yaml-cpp)

add_library(
    ${PROJECT_NAME} SHARED IMPORTED GLOBAL
)
add_library(
    3rdparty::${PROJECT_NAME} ALIAS ${PROJECT_NAME}
)
set_target_properties(
    ${PROJECT_NAME} PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/libyaml-cpp.so
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/include
)
