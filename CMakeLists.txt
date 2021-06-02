cmake_minimum_required(VERSION 3.13)

project(tcpserver CXX)

add_executable(${PROJECT_NAME})

add_subdirectory(src)

find_library(BSD_EDITLINE_LIB edit)

if (NOT BSD_EDITLINE_LIB)
   message(FATAL_ERROR "libedit not found. Try 'sudo apt install libedit-dev'")

endif()

# target_compile_definitions(${PROJECT_NAME} PUBLIC ASIO_STANDALONE)
target_link_libraries(${PROJECT_NAME} PRIVATE pthread)
target_link_libraries(${PROJECT_NAME} PRIVATE boost_thread)
target_link_libraries(${PROJECT_NAME} PRIVATE ${BSD_EDITLINE_LIB})
