add_executable(util_tests  util_tests.cpp tests_main.cpp )
target_link_libraries(util_tests boost_system monstar pthread)
add_test(util util_tests)
target_include_directories( util_tests
  PRIVATE ${PROJECT_SOURCE_DIR}/external_deps/Catch2/single_include
)

add_executable(unit_config configuration.cpp tests_main.cpp )
target_link_libraries(unit_config boost_system monstar pthread)
add_test(configuration unit_config)
target_include_directories( unit_config
  PRIVATE ${PROJECT_SOURCE_DIR}/external_deps/Catch2/single_include
)
