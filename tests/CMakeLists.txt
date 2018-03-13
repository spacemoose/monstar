include_directories("${CMAKE_SOURCE_DIR}/source")
add_executable(speed speed.cpp)
target_link_libraries(speed pthread monstar)
