cmake_minimum_required(VERSION 3.16)
project(CANvenient C)

set(PROJECT_VERSION "1.0.1")
set(PROJECT_VERSION_MAJOR "1")

include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_library(
  ${PROJECT_NAME}
  SHARED
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CANvenient.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/CANvenient_Ixxat.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/CANvenient_Kvaser.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/CANvenient_PEAK.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/CANvenient_SocketCAN.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/CANvenient_Softing.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/CANvenient_TinyCan.c
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

  set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR})
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

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(FILES include/CANvenient.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/canvenient.pc.in
  ${CMAKE_CURRENT_BINARY_DIR}/canvenient.pc
  @ONLY
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/canvenient.pc
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)
