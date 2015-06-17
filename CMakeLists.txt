cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(star-base)


# Add to path in order to pick up the FindXXX.cmake files included in this project
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")

# This project depends on ROOT
find_package(ROOT)

if(NOT ROOT_FOUND)
	message(FATAL_ERROR "Fatal error: ROOT package not found")
endif()


add_definitions(-D__ROOT__)

include_directories(
	${ROOT_INCLUDE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
	St_base
	StarRoot
)

add_subdirectory(StChain)
add_subdirectory(St_base)
add_subdirectory(StarClassLibrary)
add_subdirectory(StarRoot)
