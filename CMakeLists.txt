cmake_minimum_required(VERSION 3.25)

project(vulkan_test)


add_compile_options("--std=c++20")
set(CMAKE_CXX_FLAGS   "-g")                     # 调试信息
find_library(libglfw glfw)
find_library(libvulkan vulkan)
include_directories("/opt/homebrew/opt/glm/include")
include_directories("/opt/homebrew/opt/glfw/include/")
include_directories("/usr/local/include/")
add_executable(vulkan_test main.cpp  ApplicationBase.cpp)

target_link_libraries(vulkan_test ${libglfw} ${libvulkan})