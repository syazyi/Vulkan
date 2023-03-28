file(GLOB_RECURSE HEADER_FILE "*.h")
file(GLOB_RECURSE SOURCE_FILE "*.cpp")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HEADER_FILE} ${SOURCE_FILE})
set(TARGET_NAME Application)

add_executable(${TARGET_NAME} ${HEADER_FILE} ${SOURCE_FILE})

target_link_libraries(${TARGET_NAME} PUBLIC glfw)
target_link_libraries(${TARGET_NAME} PUBLIC glm)

target_link_libraries(${TARGET_NAME} PUBLIC ${vulkan_lib})
target_include_directories(${TARGET_NAME} PUBLIC ${vulkan_include})

target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(${TARGET_NAME} PUBLIC ${shaderc_include})
target_link_libraries(${TARGET_NAME}  PUBLIC debug ${shaderc_libd} optimized ${shaderc_lib})