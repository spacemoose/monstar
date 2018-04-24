include_directories(
  ${PROJECT_SOURCE_DIR}/source
  ${PROJECT_SOURCE_DIR}/external_deps/Catch2/single_include
)
set (SOURCES configuration.cpp tests_main.cpp )
add_executable(unit_config ${SOURCES} )
target_link_libraries(unit_config boost_system monstar)
add_test(configuration unit_config)
