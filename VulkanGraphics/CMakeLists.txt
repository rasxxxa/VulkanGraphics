# CMakeList.txt : CMake project for VulkanGraphics, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(SDL2_DIR "${CMAKE_SOURCE_DIR}/SDKS/SDL2-2.26.2" )
set(ENV{CMAKE_PREFIX_PATH} "${CMAKE_SOURCE_DIR}/SDKS/SDL2-2.26.2")

find_package(SDL2 REQUIRED)

# Add source to this project's executable.
add_executable (VulkanGraphics 
"VulkanGraphics.cpp" "VulkanGraphics.h" 
"VKInit.h"
"../Externals/vma/vk_mem_alloc.h"
"VulkanEngine.h" "VulkanEngine.cpp"  
"Helper.ixx" 
"../Externals/vkbootstrap/VkBootstrap.h"
"../Externals/vkbootstrap/VkBootstrap.cpp"
"../Externals/glm/glm/glm.hpp"
"../Externals/glm/glm/gtx/transform.hpp"
   "DeletionQueue.h" "DeletionQueue.cpp" "Mesh.h" "Mesh.cpp" "VkTypes.h")

target_link_libraries(VulkanGraphics SDL2::SDL2 SDL2::SDL2main)

file(COPY ${CMAKE_SOURCE_DIR}/SDKS/SDL2-2.26.2/lib/x64/SDL2.dll DESTINATION ${CMAKE_SOURCE_DIR}/out/build/x64-debug/VulkanGraphics)
file(COPY ${CMAKE_SOURCE_DIR}/SDKS/SDL2-2.26.2/lib/x64/SDL2.dll DESTINATION ${CMAKE_SOURCE_DIR}/out/build/x64-release/VulkanGraphics)

find_package(Vulkan REQUIRED)
target_link_libraries(VulkanGraphics Vulkan::Vulkan Vulkan::Headers)
target_include_directories(VulkanGraphics PUBLIC ${Vulkan_INCLUDE_DIRS})

message(${CMAKE_SOURCE_DIR})
target_include_directories(VulkanGraphics PUBLIC ${CMAKE_SOURCE_DIR}/Externals)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET VulkanGraphics PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
