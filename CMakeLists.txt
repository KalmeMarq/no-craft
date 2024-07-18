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

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)

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

set(source "${CMAKE_SOURCE_DIR}/res")
set(destination "${CMAKE_CURRENT_BINARY_DIR}/res")

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${source} ${destination} 
    DEPENDS ${destination}
    COMMENT "symbolic link res folder from ${source} => ${destination}"
)