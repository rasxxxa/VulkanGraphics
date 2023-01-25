# CMakeList.txt : CMake project for VulkanGraphics, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(SDL2_DIR "${CMAKE_SOURCE_DIR}/SDKS/SDL2-2.26.2" )
set(ENV{CMAKE_PREFIX_PATH} "${CMAKE_SOURCE_DIR}/SDKS/SDL2-2.26.2")

find_package(SDL2 REQUIRED)

# Add source to this project's executable.
add_executable (VulkanGraphics "VulkanGraphics.cpp" "VulkanGraphics.h" "VulkanEngine.h" "VulkanEngine.cpp")

target_link_libraries(VulkanGraphics SDL2::SDL2 SDL2::SDL2main)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET VulkanGraphics PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
