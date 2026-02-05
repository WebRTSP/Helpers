cmake_minimum_required(VERSION 3.10)

project(Helpers)

option(LIBWEBSOCKETS_SUPPORT "libwebsockets support" ON)

find_package(PkgConfig REQUIRED)

pkg_search_module(GLIB REQUIRED glib-2.0)
pkg_search_module(SPDLOG REQUIRED spdlog)
if(LIBWEBSOCKETS_SUPPORT)
    pkg_search_module(LIBWEBSOCKETS REQUIRED libwebsockets)
endif()

set(SOURCES
    EventSource.h
    EventSource.cpp
    Actor.h
    Actor.cpp
    ConfigHelpers.h
    ConfigHelpers.cpp
    TurnRestApi.h
    TurnRestApi.cpp
)

if(LIBWEBSOCKETS_SUPPORT)
    set(LIBWEBSOCKETS_RELATED_SOURCES
        LwsLog.h
        LwsLog.cpp
        MessageBuffer.h
        MessageBuffer.cpp
    )
endif()

add_library(${PROJECT_NAME}
    ${SOURCES}
    ${LIBWEBSOCKETS_RELATED_SOURCES}
)
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${GLIB_INCLUDE_DIRS}
        ${SPDLOG_INCLUDE_DIRS}
)
target_compile_options(${PROJECT_NAME}
    PUBLIC
        ${SPDLOG_CFLAGS}
)
target_link_libraries(${PROJECT_NAME}
    ${GLIB_LDFLAGS}
    ${SPDLOG_LDFLAGS}
)

if(LIBWEBSOCKETS_SUPPORT)
    target_include_directories(${PROJECT_NAME}
        PUBLIC
            ${LIBWEBSOCKETS_INCLUDE_DIRS}
    )
    target_link_libraries(${PROJECT_NAME}
        ${LIBWEBSOCKETS_LDFLAGS}
    )
endif()
