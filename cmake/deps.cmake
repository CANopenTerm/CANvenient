cmake_minimum_required(VERSION 3.16)

if(WIN32)
  # HMS Ixxat VCI C API
  set(IXXAT_VCI_VERSION "4.0.1348.0")
  
  set(IXXAT_VCI_PLATFORM  "x64")
  set(IXXAT_VCI_PATH      "${CMAKE_CURRENT_SOURCE_DIR}/deps/HMS_Ixxat_VCI")
  
  if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(IXXAT_VCI_PLATFORM "x32")
  endif()
  
  set(IXXAT_VCI_DEVEL_PKG  HMS_Ixxat_VCI-${IXXAT_VCI_VERSION}.zip)
  set(IXXAT_VCI_DEVEL_URL  https://canopenterm.de/mirror)
  set(IXXAT_VCI_DEVEL_HASH 7a6df069d6acd667631282026252d2b33b7e7507)
  
  ExternalProject_Add(Ixxat_VCI_devel
    URL ${IXXAT_VCI_DEVEL_URL}/${IXXAT_VCI_DEVEL_PKG}
    URL_HASH SHA1=${IXXAT_VCI_DEVEL_HASH}
    DOWNLOAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps
    DOWNLOAD_NO_PROGRESS true
    DOWNLOAD_EXTRACT_TIMESTAMP true
    TLS_VERIFY true
    SOURCE_DIR ${IXXAT_VCI_PATH}/
    BUILD_BYPRODUCTS ${IXXAT_VCI_PATH}/sdk/vci/lib/${IXXAT_VCI_PLATFORM}/release/vciapi.lib
  
    BUILD_COMMAND ${CMAKE_COMMAND} -E echo "Skipping build step."
  
    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy
      ${IXXAT_VCI_PATH}/sdk/vci/bin/${IXXAT_VCI_PLATFORM}/release/vciapi.dll ${CMAKE_CURRENT_BINARY_DIR}/
  
    PATCH_COMMAND ${CMAKE_COMMAND} -E copy
      "${CMAKE_CURRENT_SOURCE_DIR}/cmake/dep_ixxat.cmake" ${IXXAT_VCI_PATH}/CMakeLists.txt
  )

  set(IXXAT_VCI_INCLUDE_DIR ${IXXAT_VCI_PATH}/sdk/vci/inc)
  set(IXXAT_VCI_LIBRARY     ${IXXAT_VCI_PATH}/sdk/vci/lib/${IXXAT_VCI_PLATFORM}/release/vciapi.lib)

  # PCAN-Basic API
  set(PCAN_VERSION_WINDOWS "5.0.0.1115")
  
  set(PCAN_PLATFORM  "x64")
  set(PCAN_PATH      "${CMAKE_CURRENT_SOURCE_DIR}/deps/PCAN-Basic_API_Windows")
  
  if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(PCAN_PLATFORM "Win32")
  endif()
  
  set(PCAN_DEVEL_PKG  PCAN-Basic_Windows-${PCAN_VERSION_WINDOWS}.zip)
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

  set(PLATFORM_DEPS
    Ixxat_VCI_devel
    PCAN_devel
  )
  
  set(PLATFORM_LIBS
    ${IXXAT_VCI_LIBRARY}
    ${PCAN_LIBRARY}
  )

  include_directories(
    SYSTEM ${IXXAT_VCI_INCLUDE_DIR}
    SYSTEM ${PCAN_INCLUDE_DIR}
    SYSTEM ${PCAN_INCLUDE_DIR}/../src/pcan/driver
    SYSTEM ${PCAN_INCLUDE_DIR}/../src/pcan/lib
  )

  add_dependencies(
    ${PROJECT_NAME}
    Ixxat_VCI_devel
    PCAN_devel)

elseif(UNIX)

endif()
