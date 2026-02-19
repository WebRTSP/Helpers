cmake_minimum_required(VERSION 3.10)

project(Helpers)

option(LIBWEBSOCKETS_SUPPORT "libwebsockets support" ON)

find_package(PkgConfig REQUIRED)

pkg_search_module(GLIB REQUIRED glib-2.0)
if(LIBWEBSOCKETS_SUPPORT)
    pkg_search_module(LIBWEBSOCKETS REQUIRED libwebsockets)
endif()
if(ANDROID)
    include(FetchContent)

    FetchContent_Declare(
            spdlog
            GIT_REPOSITORY https://github.com/gabime/spdlog.git
            GIT_TAG v1.17.0
    )
    FetchContent_MakeAvailable(spdlog)
else()
    pkg_search_module(SPDLOG REQUIRED spdlog)
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
    EventSource2.h
    EventSource2.cpp
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
)
target_link_libraries(${PROJECT_NAME}
    ${GLIB_LDFLAGS}
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

if(ANDROID)
    target_link_libraries(${PROJECT_NAME}
        spdlog::spdlog
    )
else()
    target_include_directories(${PROJECT_NAME}
        PUBLIC
            ${SPDLOG_INCLUDE_DIRS}
    )
    target_compile_options(${PROJECT_NAME}
        PUBLIC
            ${SPDLOG_CFLAGS}
    )
    target_link_libraries(${PROJECT_NAME}
        ${SPDLOG_LDFLAGS}
    )
endif()
