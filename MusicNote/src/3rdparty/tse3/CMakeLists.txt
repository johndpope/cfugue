###################################################
###  Auto-Generated with GPalem CMake Generator    
###                      from                      
###  D:/WinApp/CarMusTy/src/References/CFugue/src/3rdparty/tse3/build/tse3.sln
###################################################

cmake_minimum_required(VERSION 2.8)

message(STATUS "------ tse3 ---------")

#################################
# Setup Build Options
#################################
option (TSE3_BUILD_TESTS "Should the Test applications be built for Tse3" ON) 
SET (TSE3_UNICODE_BUILD OFF CACHE BOOL "Should TSE3 be built with Unicode? (default: No)")

#################################
# Compute Values based on Options
#################################
set(Char_Flags "" CACHE INTERNAL "Compiler Character Flags: Unicode/MBCS Options")
set(Char_postfix "" CACHE INTERNAL "Unicode Postfix")
set(Output_postfix "" CACHE INTERNAL "Debug Postfix")

if(TSE3_UNICODE_BUILD)
	SET(Char_postfix "u")
	SET(Char_Flags "UNICODE;_UNICODE")
	message(STATUS "Unicode build is ON")
else(TSE3_UNICODE_BUILD)
	SET(Char_postfix "")
	SET(Char_Flags "_MBCS")
	message(STATUS "Unicode build is OFF")
endif(TSE3_UNICODE_BUILD)

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "Debug" CACHE STRING
        "Choose the type of build, options are: Debug Release"
        FORCE)   
ENDIF(NOT CMAKE_BUILD_TYPE)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
	SET(Output_postfix "")	
endif()
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	SET(Output_postfix "d")	
endif()
		
SET(CMAKE_DEBUG_POSTFIX "d${Char_postfix}")
SET(CMAKE_RELEASE_POSTFIX "${Char_postfix}")
message(STATUS "Output Postfix: ${Output_postfix}${Char_postfix}")

SET(TARGET_COMPILE_DEFS "${Char_Flags};_CRT_SECURE_NO_DEPRECATE;_CRT_SECURE_NO_WARNINGS;_BIND_TO_CURRENT_CRT_VERSION")

IF(MSVC)
  SET(TARGET_COMPILE_FLAGS "/Zc:wchar_t- /Zc:forScope")
ENDIF(MSVC)

IF(CMAKE_COMPILER_IS_GNUCC)
  SET(TARGET_COMPILE_FLAGS "-std=gnu++11") # Add C++11 support for GCC
ENDIF()

#################################
# Setup Output Directories
#################################
SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG
  ${PROJECT_SOURCE_DIR}/bin
  CACHE PATH
  "Destination Directory for output Shared Libraries (Debug Mode)"
  )
SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE
  ${PROJECT_SOURCE_DIR}/bin
  CACHE PATH
  "Destination Directory for output Shared Libraries (Release Mode)"
  )

SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG
  ${PROJECT_SOURCE_DIR}/bin
  CACHE PATH
  "Destination Directory for output Executables (Debug Mode)."
  )
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE
  ${PROJECT_SOURCE_DIR}/bin
  CACHE PATH
  "Destination Directory for output Executables (Release Mode)."
  )

SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG
  ${PROJECT_SOURCE_DIR}/lib
  CACHE PATH
  "Destination Directory for output static libraries (Debug Mode)."
  )
SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE
  ${PROJECT_SOURCE_DIR}/lib
  CACHE PATH
  "Destination Directory for output static libraries (Release Mode)."
  )

mark_as_advanced(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG)
mark_as_advanced(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE)
mark_as_advanced(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG)
mark_as_advanced(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE)
mark_as_advanced(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG)
mark_as_advanced(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../include/)

include(cmd.cmake)
include(tse3file.cmake)
include(ins.cmake)
include(tse3.cmake)
#include(listen.cmake)

if(TSE3_BUILD_TESTS)
	include(tse3ports.cmake)
	include(tse3MidiFile.cmake)
	include(tseplay.cmake)
endif(TSE3_BUILD_TESTS)
