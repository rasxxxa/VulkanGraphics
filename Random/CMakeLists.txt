# CMakeList.txt : CMake project for VulkanGraphics, include source and define
# project specific logic here.
#

set(PROJECT_NAME Random)
set(ALL_FILES 
Random.h
Random.cpp
)
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})



cmake_minimum_required (VERSION 3.8)
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Random PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
