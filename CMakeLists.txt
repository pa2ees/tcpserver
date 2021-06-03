cmake_minimum_required(VERSION 3.13)

project(commsserver CXX)

add_library(${PROJECT_NAME} STATIC)

add_subdirectory(src)

target_include_directories(${PROJECT_NAME} PUBLIC include)

target_link_libraries(${PROJECT_NAME} PRIVATE pthread)
target_link_libraries(${PROJECT_NAME} PRIVATE boost_thread)

add_subdirectory(examples)
