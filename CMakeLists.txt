cmake_minimum_required(VERSION 3.5)

project(Helpers)

find_package(PkgConfig REQUIRED)
pkg_search_module(WS REQUIRED libwebsockets)
pkg_search_module(GLIB REQUIRED glib-2.0)
pkg_search_module(SPDLOG REQUIRED spdlog)

file(GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    [^.]*.cpp
    [^.]*.h
    [^.]*.cmake
    )

add_library(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC
    ${WS_INCLUDE_DIRS}
    ${GLIB_INCLUDE_DIRS}
    ${SPDLOG_INCLUDE_DIRS})
target_compile_options(${PROJECT_NAME} PUBLIC ${SPDLOG_CFLAGS})
target_link_libraries(${PROJECT_NAME}
    ${WS_LDFLAGS}
    ${GLIB_LDFLAGS}
    ${SPDLOG_LDFLAGS})
