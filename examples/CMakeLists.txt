include_directories("${CMAKE_SOURCE_DIR}/source")

add_executable(silly_example 0_silly_example.cpp)
add_executable(less_silly  1_less_silly.cpp)
add_executable(sandbox sandbox.cpp)

set(example_libs pthread monstar)

target_link_libraries(silly_example ${example_libs})
target_link_libraries(less_silly ${example_libs})
target_link_libraries(sandbox ${example_libs})
