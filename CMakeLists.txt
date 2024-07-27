cmake_minimum_required(VERSION 3.10)
project(CANvenient C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_CURRENT_SOURCE_DIR}/export)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/export)

include(FetchContent)

set(CMAKE_C_STANDARD 90)

if (POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

if(WIN32)
  set(PLATFORM "windows")
elseif(UNIX)
  set(PLATFORM "linux")
else()
  message(FATAL_ERROR "Unsupported platform")
endif()

set(core_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CANvenient.c)

add_library(
  CANvenient
  STATIC
  ${core_sources})

include_directories(
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (CMAKE_C_COMPILER_ID STREQUAL "Clang")
    target_compile_options(
        ${PROJECT_NAME}
        PUBLIC
        -Wall
        -Wextra
        -Wpedantic
        -Wno-unsafe-buffer-usage
        -Wno-covered-switch-default
        -Werror)

    if (ENABLE_ADDRESS_SANITIZER)
        target_compile_options(
            ${PROJECT_NAME}
            PUBLIC
            -fsanitize=address)

        target_link_options(
            ${PROJECT_NAME}
            PUBLIC
            -fsanitize=address)

        target_link_libraries(
            ${PROJECT_NAME}
            clang_rt.asan_dynamic-x86_64
            clang_rt.asan_dynamic_runtime_thunk-x86_64)

    endif()

elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    target_compile_options(
        ${PROJECT_NAME}
        PUBLIC
        -Wall
        -Wextra
        -Wpedantic)

elseif (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(
        ${PROJECT_NAME}
        PUBLIC
        /W4
        /utf-8)

endif()
