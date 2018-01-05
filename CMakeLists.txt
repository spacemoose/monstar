cmake_minimum_required(VERSION 3.6)
set (CMAKE_CXX_STANDARD 14)
project(monstar)

add_definitions(-Wall)
add_subdirectory(source)
add_subdirectory(examples)

#--------------
# Packaging
# the next two lines can be ditched when we can count on cmake >3.6
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A library for monitoring C++ applications in Grafana")
set(VERSION 0.01 )
set(CPACK_GENERATOR "RPM")
set(CPACK_PACKAGE_NAME "monstar")
set(CPACK_RPM_PACKAGE_RELEASE 1)
set(CPACK_RPM_PACKAGE_LICENSE "LGPL-3.0")
set(CPACK_PACKAGE_CONTACT "Glen Stark")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/CPack.desc")
set(CPACK_PACKAGING_INSTALL_PREFIX /usr)
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${VERSION}.${CMAKE_SYSTEM_PROCESSOR}")
include(CPack)


#---------------
# Documentation.
option(BUILD_DOC "Build documentation" ON)
find_package(Doxygen)
if(DOXYGEN_FOUND)
  set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
  set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Docs)
  configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
  message("starting Doxygen build")
  add_custom_target(doc_doxygen ALL
	COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMENT "Generating API documentation with Doxygen"
	VERBATIM)
else (DOXYGEN_FOUND)
  message("Doxygen not installed")
endif(DOXYGEN_FOUND)
