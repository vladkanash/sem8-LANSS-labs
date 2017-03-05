cmake_minimum_required(VERSION 3.6)

set(SOURCE_FILES chat.c member_list.c)
set(HEADER_FILES types.h member_list.h)
add_executable(LANSS_lab2 ${SOURCE_FILES} ${HEADER_FILES})

find_package(Threads)
target_link_libraries(LANSS_lab2 ${CMAKE_THREAD_LIBS_INIT})

if(WIN32)
    target_link_libraries(LANSS_lab2 wsock32 ws2_32)
endif()
