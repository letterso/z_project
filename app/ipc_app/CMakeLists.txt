add_executable(pub_node
   ./pub_node.cpp)
target_link_libraries(pub_node PUBLIC
    IPC_LIBS
    COMMON_LIBS
)

add_executable(sub_node
   ./sub_node.cpp)
target_link_libraries(sub_node PUBLIC
   IPC_LIBS
   COMMON_LIBS
)