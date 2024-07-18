cmake_minimum_required(VERSION 3.30)

project(km-opengl LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)

set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "")

add_subdirectory(libs/glfw)
add_subdirectory(libs/glad)
add_subdirectory(libs/glm)

include_directories("libs/stb")

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)

add_executable(
    km-opengl
    ${SOURCES}
)

target_link_libraries(
    km-opengl
    glfw
    glm
    glad    
)

target_compile_definitions(km-opengl PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/res/")
# target_compile_definitions(km-opengl PUBLIC RESOURCES_PATH="./res/")
