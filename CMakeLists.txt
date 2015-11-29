LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmakemodules)

set(TS_VS_CONFIG ${CMAKE_BUILD_TYPE})
IF(WIN32)
	cmake_minimum_required(VERSION 3.1.3)
	set_property(GLOBAL PROPERTY USE_FOLDERS On)
ELSE()
	cmake_minimum_required(VERSION 3.1.3)
ENDIF()

project (OpenVEILnodeJS C CXX)

IF(WIN32)
	if(MSVC_IDE)
		set(TS_INSTALL_PREFIX       "c:/TecSec/OpenVEIL_7-0")
	else(MSVC_IDE)
		set(TS_INSTALL_PREFIX       "c:/TecSec/OpenVEIL_7-0")
	endif(MSVC_IDE)
	set(TS_INSTALL_PREFIX_DEBUG   "c:/TecSec/OpenVEIL_7-0")
	set(TS_INSTALL_PREFIX_RELEASE "c:/TecSec/OpenVEIL_7-0")
ELSE(WIN32)
	SET(TS_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/../local" CACHE STRING "The install path to use for Linux")
ENDIF(WIN32)

set(TS_MODULE OpenVEILnodeJs)

include(tecsec_top)
include(precompiled.header)
set(TS_INSTALL_PREFIX ${TS_INSTALL_PREFIX}/${TS_TOOLSET})
include (tecsec_installdirs)

include(summary)
include(${ALLBIN_DIR}/OpenVEILVersion.cmake)
set(CMAKE_DEBUG_POSTFIX "")

message(STATUS "ALLBIN_DIR = ${ALLBIN_DIR}")
message(STATUS "OPENVEIL_INSTALL_BASE_DIR = ${OPENVEIL_INSTALL_BASE_DIR}")
message(STATUS "OPENVEIL_INSTALL_BIN_DIR = ${OPENVEIL_INSTALL_BIN_DIR}")
message(STATUS "OPENVEIL_INSTALL_INCLUDE_DIR = ${OPENVEIL_INSTALL_INCLUDE_DIR}")
message(STATUS "OPENVEIL_INSTALL_LIB_DIR = ${OPENVEIL_INSTALL_LIB_DIR}")
message(STATUS "OPENVEIL_INSTALL_SHLIB_DIR = ${OPENVEIL_INSTALL_SHLIB_DIR}")


ImportTarget(OpenVEILCore)
ImportTarget(CmsHeader)
ImportTarget(FileVEILSupport)

set(CMAKE_BUILD_TYPE ${TS_VS_CONFIG} CACHE STRING "The configuration build type")
if (CMAKE_CONFIGURATION_TYPES)
  set (CMAKE_CONFIGURATION_TYPES ${TS_VS_CONFIG})
  set (CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING "Reset the configurations for ${VEILCORENAME}" )
endif()

set(VEIL_COPYRIGHT "Copyright (C) 2015 TecSec, Inc.  All Rights Reserved Worldwide.")

include_directories(${CMAKE_JS_INC})

add_uninstall()

add_library(OpenVEIL SHARED "src/KeyVEILConnector.cpp" "src/KeyVEILConnector.h" "src/GenericConnector.cpp" "src/GenericConnector.h" "src/Favorite.cpp" "src/Favorite.h" "src/Session.cpp" "src/Session.h" "src/Token.cpp" "src/Token.h" "src/addon.cc")
set_target_properties(OpenVEIL PROPERTIES PREFIX "" SUFFIX ".node")
target_link_libraries(OpenVEIL ${CMAKE_JS_LIB} OpenVEILCore CmsHeader FileVEILSupport)

summary()

CopyImportTargetBinaries(OpenVEILCore ${BIN_DIR})
CopyImportTargetBinaries(CmsHeader ${BIN_DIR})
CopyImportTargetBinaries(FileVEILSupport ${BIN_DIR})

install(TARGETS OpenVEIL ARCHIVE DESTINATION ${LIB_DIR} LIBRARY DESTINATION ${SHLIB_DIR} RUNTIME DESTINATION ${BIN_DIR} )
install(FILES Samples/tokenList.js DESTINATION ${BIN_DIR}/Samples)
# DumpAllVariables()
