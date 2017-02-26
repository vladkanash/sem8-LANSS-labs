cmake_minimum_required(VERSION 3.6)

set(SOURCE_FILES ping.c utils.c)
set(HEADER_FILES types.h utils.h)
add_executable(LANSS_lab1 ${SOURCE_FILES} ${HEADER_FILES})

if(WIN32)
    target_link_libraries(LANSS_lab1 wsock32 ws2_32)
endif()
