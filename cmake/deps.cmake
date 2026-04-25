cmake_minimum_required(VERSION 3.16)

if(WIN32)
  # HMS Ixxat VCI C API
  set(IXXAT_VERSION "4.0.1348.0")

  set(IXXAT_PLATFORM  "x64")
  set(IXXAT_PATH      "${CMAKE_CURRENT_SOURCE_DIR}/deps/HMS_Ixxat_VCI")

  if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(IXXAT_PLATFORM "x32")
  endif()

  set(IXXAT_DEVEL_PKG  HMS_Ixxat_VCI-${IXXAT_VERSION}.zip)
  set(IXXAT_DEVEL_URL  https://canopenterm.de/mirror)
  set(IXXAT_DEVEL_HASH 7a6df069d6acd667631282026252d2b33b7e7507)

  ExternalProject_Add(Ixxat_devel
    URL ${IXXAT_DEVEL_URL}/${IXXAT_DEVEL_PKG}
    URL_HASH SHA1=${IXXAT_DEVEL_HASH}
    DOWNLOAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps
    DOWNLOAD_NO_PROGRESS true
    DOWNLOAD_EXTRACT_TIMESTAMP true
    TLS_VERIFY true
    SOURCE_DIR ${IXXAT_PATH}/
    BUILD_BYPRODUCTS ${IXXAT_PATH}/sdk/vci/lib/${IXXAT_PLATFORM}/release/vciapi.lib

    BUILD_COMMAND ${CMAKE_COMMAND} -E echo "Skipping build step."

    INSTALL_COMMAND ${CMAKE_COMMAND} -E echo "Skipping install step."

    PATCH_COMMAND ${CMAKE_COMMAND} -E copy
      "${CMAKE_CURRENT_SOURCE_DIR}/cmake/dep_ixxat.cmake" ${IXXAT_PATH}/CMakeLists.txt
  )

  set(IXXAT_INCLUDE_DIR ${IXXAT_PATH}/sdk/vci/inc)
  set(IXXAT_LIBRARY     ${IXXAT_PATH}/sdk/vci/lib/${IXXAT_PLATFORM}/release/vciapi.lib)

  # Kvaser CANlib API
  set(KVASER_VERSION "5.51.461")

  set(KVASER_PLATFORM  "x64")
  set(KVASER_PATH      "${CMAKE_CURRENT_SOURCE_DIR}/deps/Kvaser")
  set(KVASER_BINDIR    "bin_x64")

  if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(KVASER_PLATFORM "MS")
    set(KVASER_BINDIR   "Bin")
  endif()

  set(KVASER_DEVEL_PKG  Kvaser_Canlib-${KVASER_VERSION}.zip)
  set(KVASER_DEVEL_URL  https://canopenterm.de/mirror)
  set(KVASER_DEVEL_HASH c73a6ea7cf981b94d0685e561a801dbc840febcc)

  ExternalProject_Add(Kvaser_devel
    URL ${KVASER_DEVEL_URL}/${KVASER_DEVEL_PKG}
    URL_HASH SHA1=${KVASER_DEVEL_HASH}
    DOWNLOAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps
    DOWNLOAD_NO_PROGRESS true
    DOWNLOAD_EXTRACT_TIMESTAMP true
    TLS_VERIFY true
    SOURCE_DIR ${KVASER_PATH}/
    BUILD_BYPRODUCTS ${KVASER_PATH}/Canlib/Lib/${KVASER_PLATFORM}/canlib32.lib

    BUILD_COMMAND ${CMAKE_COMMAND} -E echo "Skipping build step."

    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy
      ${KVASER_PATH}/Canlib/${KVASER_BINDIR}/canlib32.dll ${CMAKE_CURRENT_BINARY_DIR}/

    PATCH_COMMAND ${CMAKE_COMMAND} -E copy
      "${CMAKE_CURRENT_SOURCE_DIR}/cmake/dep_kvaser.cmake" ${KVASER_PATH}/CMakeLists.txt
  )

  set(KVASER_INCLUDE_DIR ${KVASER_PATH}/Canlib/INC)
  set(KVASER_LIBRARY     ${KVASER_PATH}/Canlib/Lib/${KVASER_PLATFORM}/canlib32.lib)

  # PCAN-Basic API
  set(PCAN_VERSION "5.0.0.1115")

  set(PCAN_PLATFORM  "x64")
  set(PCAN_PATH      "${CMAKE_CURRENT_SOURCE_DIR}/deps/PCAN-Basic_API_Windows")

  if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(PCAN_PLATFORM "x86")
  endif()

  set(PCAN_DEVEL_PKG  PCAN-Basic_Windows-${PCAN_VERSION}.zip)
  set(PCAN_DEVEL_URL  https://canopenterm.de/mirror)
  set(PCAN_DEVEL_HASH aca36ff1a766839ffc698f6d1a1d0870f01e395e)

  ExternalProject_Add(PCAN_devel
    URL ${PCAN_DEVEL_URL}/${PCAN_DEVEL_PKG}
    URL_HASH SHA1=${PCAN_DEVEL_HASH}
    DOWNLOAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps
    DOWNLOAD_NO_PROGRESS true
    DOWNLOAD_EXTRACT_TIMESTAMP true
    TLS_VERIFY true
    SOURCE_DIR ${PCAN_PATH}/
    BUILD_BYPRODUCTS ${PCAN_PATH}/${PCAN_PLATFORM}/VC_LIB/PCANBasic.lib

    BUILD_COMMAND ${CMAKE_COMMAND} -E echo "Skipping build step."

    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy
      ${PCAN_PATH}/${PCAN_PLATFORM}/PCANBasic.dll ${CMAKE_CURRENT_BINARY_DIR}/

    PATCH_COMMAND ${CMAKE_COMMAND} -E copy
      "${CMAKE_CURRENT_SOURCE_DIR}/cmake/dep_pcan.cmake" ${PCAN_PATH}/CMakeLists.txt
  )

  set(PCAN_INCLUDE_DIR ${PCAN_PATH}/Include)
  set(PCAN_LIBRARY     ${PCAN_PATH}/${PCAN_PLATFORM}/VC_LIB/PCANBasic.lib)

  # Softing CAN Layer 2
  set(SOFTING_VERSION "5.23.1")

  set(SOFTING_PLATFORM  "Win64")
  set(SOFTING_POSTFIX   "_64")
  set(SOFTING_PATH      "${CMAKE_CURRENT_SOURCE_DIR}/deps/Softing")

  if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(SOFTING_PLATFORM "Win32")
    set(SOFTING_POSTFIX "")
  endif()

  set(SOFTING_DEVEL_PKG  Softing_CAN_Layer2-${SOFTING_VERSION}.zip)
  set(SOFTING_DEVEL_URL  https://canopenterm.de/mirror)
  set(SOFTING_DEVEL_HASH a690c72c326140ae888ad65695fcdd330e86ce0f)

  ExternalProject_Add(Softing_devel
    URL ${SOFTING_DEVEL_URL}/${SOFTING_DEVEL_PKG}
    URL_HASH SHA1=${SOFTING_DEVEL_HASH}
    DOWNLOAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps
    DOWNLOAD_NO_PROGRESS true
    DOWNLOAD_EXTRACT_TIMESTAMP true
    TLS_VERIFY true
    SOURCE_DIR ${SOFTING_PATH}/
    BUILD_BYPRODUCTS "${SOFTING_PATH}/CAN/CAN Layer2/APIDLL/${SOFTING_PLATFORM}/canL2${SOFTING_POSTFIX}.lib"

    BUILD_COMMAND ${CMAKE_COMMAND} -E echo "Skipping build step."

    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy
      "${SOFTING_PATH}/CAN/CAN Layer2/APIDLL/${SOFTING_PLATFORM}/canL2${SOFTING_POSTFIX}.dll"
      "${SOFTING_PATH}/CAN/CAN Layer2/APIDLL/${SOFTING_PLATFORM}/CANusbM.dll"
      "${SOFTING_PATH}/CAN/CAN Layer2/APIDLL/Win64/canchd_64.dll"
      ${CMAKE_CURRENT_BINARY_DIR}/

    PATCH_COMMAND ${CMAKE_COMMAND} -E copy
      "${CMAKE_CURRENT_SOURCE_DIR}/cmake/dep_softing.cmake" ${SOFTING_PATH}/CMakeLists.txt
  )

  set(SOFTING_INCLUDE_DIR "${SOFTING_PATH}/CAN/CAN Layer2/APIDLL")
  set(SOFTING_LIBRARY     "${SOFTING_PATH}/CAN/CAN Layer2/APIDLL/${SOFTING_PLATFORM}/canL2${SOFTING_POSTFIX}.lib")

  set(PLATFORM_DEPS
    Ixxat_devel
    Kvaser_devel
    PCAN_devel
    Softing_devel
  )

  set(PLATFORM_LIBS
    ${IXXAT_LIBRARY}
    ${KVASER_LIBRARY}
    ${PCAN_LIBRARY}
    ${SOFTING_LIBRARY}
  )

  include_directories(
    SYSTEM ${IXXAT_INCLUDE_DIR}
    SYSTEM ${KVASER_INCLUDE_DIR}
    SYSTEM ${KVASER_INCLUDE_DIR}/extras
    SYSTEM ${PCAN_INCLUDE_DIR}
    SYSTEM ${PCAN_INCLUDE_DIR}/../src/pcan/driver
    SYSTEM ${PCAN_INCLUDE_DIR}/../src/pcan/lib
    SYSTEM ${SOFTING_INCLUDE_DIR}
  )

  add_dependencies(
    ${PROJECT_NAME}
    Ixxat_devel
    Kvaser_devel
    PCAN_devel
    Softing_devel
  )
endif()
