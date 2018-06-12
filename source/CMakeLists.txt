set(VERSION "${VERSION}" PARENT_SCOPE)
add_library(monstar SHARED
  ${CMAKE_CURRENT_LIST_DIR}/monstar.cpp
  ${CMAKE_CURRENT_LIST_DIR}/notification.cpp
  ${CMAKE_CURRENT_LIST_DIR}/notification_handler.cpp
  ${CMAKE_CURRENT_LIST_DIR}/simple_recorder.cpp
)

add_subdirectory(detail)
add_subdirectory(tests)

target_include_directories(monstar PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC SYSTEM ${PROJECT_SOURCE_DIR}/external_deps/spdlog/include/
  PUBLIC SYSTEM ${PROJECT_SOURCE_DIR}/external_deps/fmt/
)

set_target_properties(monstar PROPERTIES PUBLIC_HEADER
  "monstar.hpp;notification.hpp;notification_handler.hpp;io_ops.hpp;simple_recorder.hpp;epoch.hpp")
target_link_libraries (monstar boost_system )

install(TARGETS monstar DESTINATION lib64 PUBLIC_HEADER DESTINATION include/monstar)
