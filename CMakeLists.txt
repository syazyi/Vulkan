cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

project(vulkanStudy VERSION 0.1)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(DEMO_ROOR_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(3RDPART_DIRECTORY ${DEMO_ROOR_DIRECTORY}/3rdPart)
set(vulkan_lib ${3RDPART_DIRECTORY}/VulkanSDK/libs/vulkan-1.lib)
set(vulkan_include ${3RDPART_DIRECTORY}/VulkanSDK/include)
set(vulkan_lib_directory ${3RDPART_DIRECTORY}/VulkanSDK/libs)

set(shaderc_include ${3RDPART_DIRECTORY}/shaderc/include)

set(shaderc_libd ${3RDPART_DIRECTORY}/shaderc/lib/shaderc_combinedd.lib)
set(shaderc_lib ${3RDPART_DIRECTORY}/shaderc/lib/shaderc_combined.lib)

set(stb_image_header ${3RDPART_DIRECTORY}/stb)

set(tinyobjloader ${3RDPART_DIRECTORY}/tinyobjloader)

#set output dir
set(EXECUTABLE_OUTPUT_PATH ${DEMO_ROOR_DIRECTORY}/bin)
add_subdirectory(3rdPart)

add_subdirectory(Applicaiton)

