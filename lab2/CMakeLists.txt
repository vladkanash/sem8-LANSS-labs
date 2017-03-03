cmake_minimum_required(VERSION 3.6)

set(SOURCE_FILES chat.c)
set(HEADER_FILES types.h)
add_executable(LANSS_lab2 ${SOURCE_FILES} ${HEADER_FILES})

if(WIN32)
    target_link_libraries(LANSS_lab2 wsock32 ws2_32)
endif()
