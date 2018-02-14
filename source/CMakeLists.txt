add_library(monstar SHARED
  ${CMAKE_CURRENT_LIST_DIR}/monstar.cpp
  ${CMAKE_CURRENT_LIST_DIR}/notification.cpp
  ${CMAKE_CURRENT_LIST_DIR}/notification_handler.cpp
  ${CMAKE_CURRENT_LIST_DIR}/simple_recorder.cpp
)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(detail)

target_include_directories(monstar PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(monstar PROPERTIES PUBLIC_HEADER
  "monstar.hpp;notification.hpp;notification_handler.hpp;io_ops.hpp;simple_recorder.hpp;epoch.hpp")
target_link_libraries (monstar boost_system fmt)

install(TARGETS monstar DESTINATION lib64 PUBLIC_HEADER DESTINATION include/monstar)
