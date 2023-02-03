# CMakeList.txt : CMake project for VulkanGraphics, include source and define
# project specific logic here.
#

set(PROJECT_NAME Chess)
set(ALL_FILES 
main.cpp
ChessBoard.h
ChessBoard.cpp
)

add_executable(Chess ${ALL_FILES})

target_link_libraries(Chess VulkanGraphics)
target_include_directories(Chess PUBLIC VulkanGraphics ${CMAKE_SOURCE_DIR}/Externals)


cmake_minimum_required (VERSION 3.8)
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Chess PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
