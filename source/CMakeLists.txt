add_library(monstar SHARED
  ${CMAKE_CURRENT_LIST_DIR}/monstar.cpp
  ${CMAKE_CURRENT_LIST_DIR}/TS_message.cpp
)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(detail)

target_include_directories(monstar PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(monstar PROPERTIES PUBLIC_HEADER "monstar.hpp;TS_message.hpp;")
target_link_libraries (monstar boost_system)
install(TARGETS monstar DESTINATION monstar PUBLIC_HEADER DESTINATION monstar)
