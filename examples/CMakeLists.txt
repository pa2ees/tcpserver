cmake_minimum_required(VERSION 3.13)

project(exampleApp CXX)

add_executable(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PRIVATE commsserver)

add_subdirectory(src)

