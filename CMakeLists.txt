# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

message("Downloading Vcpkg...")
include(FetchContent)
FetchContent_Declare(
  vcpkg  
  GIT_REPOSITORY https://github.com/microsoft/vcpkg.git
)

FetchContent_MakeAvailable(vcpkg)

set(CMAKE_TOOLCHAIN_FILE ${vcpkg_SOURCE_DIR}/scripts/buildsystems/vcpkg.cmake
    CACHE STRING "CMake toolchain file")

project ("VulkanGraphics")
project ("Random")
project ("Chess")

# Include sub-projects.

add_subdirectory ("VulkanGraphics")
add_subdirectory ("Random")
add_subdirectory ("Chess")

set (CMAKE_CXX_STANDARD 20)