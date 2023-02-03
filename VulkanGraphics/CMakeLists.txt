# CMakeList.txt : CMake project for VulkanGraphics, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(PROJECT_NAME VulkanGraphics)

# Add source to this project's executable.
set (ALL_FILES 
"VulkanGraphics.cpp" "VulkanGraphics.h" 
"VKInit.h"
"../Externals/vma/vk_mem_alloc.h"
"VulkanEngine.h" "VulkanEngine.cpp"  
"Helper.ixx" 
"../Externals/vkbootstrap/VkBootstrap.h"
"../Externals/vkbootstrap/VkBootstrap.cpp"
"../Externals/glm/glm/glm.hpp"
"../Externals/glm/glm/gtx/transform.hpp"
"${CMAKE_SOURCE_DIR}/imgui/imgui.h"
"${CMAKE_SOURCE_DIR}/imgui/imgui.cpp"

"${CMAKE_SOURCE_DIR}/imgui/imgui_demo.cpp"
"${CMAKE_SOURCE_DIR}/imgui/imgui_draw.cpp"
"${CMAKE_SOURCE_DIR}/imgui/imgui_widgets.cpp"

"${CMAKE_SOURCE_DIR}/imgui/imgui_impl_vulkan.cpp"
"${CMAKE_SOURCE_DIR}/imgui/imgui_impl_sdl.cpp"
   "DeletionQueue.h" "DeletionQueue.cpp" "Mesh.h" "Mesh.cpp" "VkTypes.h" "Renderable.h" "Renderable.cpp")

   add_library(${PROJECT_NAME} STATIC ${ALL_FILES})


find_package(Vulkan REQUIRED)
target_link_libraries(VulkanGraphics PUBLIC Vulkan::Vulkan Vulkan::Headers)
find_package(SDL2 REQUIRED)
target_link_libraries(VulkanGraphics PUBLIC SDL2::SDL2 SDL2::SDL2main Random)

target_include_directories(VulkanGraphics PUBLIC ${Vulkan_INCLUDE_DIRS} Random ${CMAKE_SOURCE_DIR}/imgui)
target_include_directories(VulkanGraphics PUBLIC ${CMAKE_SOURCE_DIR}/Externals)



if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET VulkanGraphics PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
