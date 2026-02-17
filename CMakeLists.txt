cmake_minimum_required(VERSION 3.10)

project(Helpers)

option(LIBWEBSOCKETS_SUPPORT "libwebsockets support" ON)

if(WIN32)
else()
    find_package(PkgConfig REQUIRED)
endif()

if(ANDROID OR WIN32)
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

if(WIN32)
    set(GSTREAMER_ROOT_DIR "$ENV{GSTREAMER_1_0_ROOT_MSVC_X86_64}")
    set(GLIB_INCLUDE_DIRS
        "${GSTREAMER_ROOT_DIR}/include/glib-2.0"
        "${GSTREAMER_ROOT_DIR}/lib/glib-2.0/include"
    )
    set(GLIB_LIBRARIES
        "${GSTREAMER_ROOT_DIR}/lib/glib-2.0.lib"
    )
else()
    pkg_search_module(GLIB REQUIRED glib-2.0)
    if(LIBWEBSOCKETS_SUPPORT)
        pkg_search_module(LIBWEBSOCKETS REQUIRED libwebsockets)
    endif()
endif()

set(SOURCES
    ConfigHelpers.h
    ConfigHelpers.cpp
    TurnRestApi.h
    TurnRestApi.cpp
    EventSource2.h
    EventSource2.cpp
)

if(WIN32)
else()
    set(ACTOR_SOURCES
        EventSource.h
        EventSource.cpp
        Actor.h
        Actor.cpp
    )
endif()

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
    ${ACTOR_SOURCES}
    ${LIBWEBSOCKETS_RELATED_SOURCES}
)
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${GLIB_INCLUDE_DIRS}
)

if(WIN32)
    target_link_libraries(${PROJECT_NAME}
        ${GLIB_LIBRARIES}
    )
else()
    target_link_libraries(${PROJECT_NAME}
        ${GLIB_LDFLAGS}
    )
endif()

if(LIBWEBSOCKETS_SUPPORT)
    target_include_directories(${PROJECT_NAME}
        PUBLIC
            ${LIBWEBSOCKETS_INCLUDE_DIRS}
    )
    target_link_libraries(${PROJECT_NAME}
        ${LIBWEBSOCKETS_LDFLAGS}
    )
endif()

if(ANDROID OR WIN32)
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
