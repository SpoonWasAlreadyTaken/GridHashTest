cmake_minimum_required(VERSION 4.3.1)
project(particleSim)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

find_package(SFML COMPONENTS Graphics Window System REQUIRED)

add_executable(${PROJECT_NAME} GridHashTest.cpp)

target_link_libraries(${PROJECT_NAME} SFML::Graphics SFML::Window SFML::System)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Assets ${CMAKE_BINARY_DIR}/Assets)

message(STATUS "Type: ${CMAKE_BUILD_TYPE}")

