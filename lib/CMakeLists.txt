cmake_minimum_required(VERSION 2.6)

add_subdirectory (resources)
IF (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/suri/)
	add_subdirectory (suri)
ENDIF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/suri/)
