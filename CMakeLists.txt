cmake_minimum_required(VERSION 3.16)
project(CANvenient C)

include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_library(
  ${PROJECT_NAME}
  SHARED
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CANvenient.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/CANvenient_SocketCAN.c
)

set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 99)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/deps.cmake)

target_link_libraries(
  ${PROJECT_NAME}
  ${PLATFORM_LIBS}
)

if(UNIX)
  target_link_libraries(
    ${PROJECT_NAME}
    socketcan
  )
endif()

if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
  target_compile_options(
    ${PROJECT_NAME}
    PUBLIC
    /W4
    /utf-8
    /wd4201 # nonstandard extension used: nameless struct/union
  )

else()
  target_compile_options(
    ${PROJECT_NAME}
    PUBLIC
    -Wall
    -Wextra
    -Wpedantic
  )

endif()

add_executable(
  example
  ${CMAKE_CURRENT_SOURCE_DIR}/src/tests/example.c
)

add_dependencies(
  example
  ${PROJECT_NAME}
)

target_link_libraries(
  example
  ${PROJECT_NAME}
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
