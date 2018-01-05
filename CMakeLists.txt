cmake_minimum_required(VERSION 2.8)
project(ITSdemo)

set(CMAKE_C_STANDARD 99)
find_package(Threads)


set(SOURCE_FILES main.c util.c util.h client.h client.c its_interface.h its_interface.c jRead.c )
add_executable(ITSdemo ${SOURCE_FILES})
target_link_libraries(ITSdemo ${CMAKE_THREAD_LIBS_INIT})