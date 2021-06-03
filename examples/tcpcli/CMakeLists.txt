cmake_minimum_required(VERSION 3.13)

add_executable(tcpcli_server EXCLUDE_FROM_ALL)

# add this example as a dependency of the all examples target
add_dependencies(${ALL_EXAMPLES_TARGET} tcpcli_server)

target_link_libraries(tcpcli_server PRIVATE commsserver)

add_subdirectory(src)
