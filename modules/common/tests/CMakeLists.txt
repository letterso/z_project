function(add_common_test target_name)
  add_executable(${target_name} ${target_name}.cpp)
  target_link_libraries(${target_name}
    PRIVATE
      COMMON_LIBS
      3rdparty::doctest
  )
  set_target_properties(${target_name} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests
  )
  add_test(NAME ${target_name} COMMAND ${target_name})
endfunction()

add_common_test(test_safe_queue)
add_common_test(test_thread_pool)
add_common_test(test_singleton)
add_common_test(test_timecost)
add_common_test(test_config)
add_common_test(test_file_manager)
add_common_test(test_timer)
