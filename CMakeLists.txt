cmake_minimum_required(VERSION 2.8.9)
project(mcxt)

include_directories(include)

add_subdirectory(src)

add_executable(main main.cpp)

# link libs to the program entry
target_link_libraries(main libcxt libcxtdata)