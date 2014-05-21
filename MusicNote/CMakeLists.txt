# CMakeLists.txt - Created with Sln2CMake Application


cmake_minimum_required(VERSION 2.6)

PROJECT( CFugue )

#################################
# Project Info
#################################
SET(PROJECT_NAME_SHORT   "CFugue")
SET(PROJECT_NAME_LONG    "CFugue")
SET(PROJECT_DESCRIPTION  "C++ Runtime Environment for MIDI Score Programming")
SET(PROJECT_COPYRIGHT    "Copyright (C) 2009 Gopalakrishna Palem")
SET(PROJECT_CONTACT      "Gopalakrishna.Palem@Yahoo.com")
SET(PROJECT_VENDOR       "Gopalakrishna Palem")
SET(ORG_WEBSITE          "http://cfugue.sourceforge.net/")


#################################
# Setup Build Options
#################################
option (CFUGUE_BUILD_DLL "Should the DLL be built for CFugue (Only for Windows)" OFF) 
option (CFUGUE_BUILD_TESTS "Should the Test applications be built for CFugue" ON) 
OPTION (CFUGUE_VERBOSE_TRACE "Should CFugue generate Verbose String Parsing Traces" ON)
SET (CFUGUE_UNICODE_BUILD ON CACHE BOOL "Should CFugue be built with Unicode? (recommended: Yes)")

mark_as_advanced(CFUGUE_BUILD_DLL)

#################################
# Compute Values based on Options
#################################
set(Char_Flags "" CACHE INTERNAL "Compiler Character Flags: Unicode/MBCS Options")
set(Char_postfix "" CACHE INTERNAL "Unicode Postfix")
set(Output_postfix "" CACHE INTERNAL "Debug Postfix")

if(CFUGUE_UNICODE_BUILD)
	SET(Char_postfix "u")
	SET(Char_Flags "UNICODE;_UNICODE")
	message(STATUS "CFugue: Unicode build is ON")
else(CFUGUE_UNICODE_BUILD)
	SET(Char_postfix "")
	SET(Char_Flags "_MBCS;")
	message(STATUS "CFugue: Unicode build is OFF")
endif(CFUGUE_UNICODE_BUILD)

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
message(STATUS "CFugue: Output Postfix: ${Output_postfix}${Char_postfix}")

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

#################################
# Generic Compiler options
#################################

SET(TARGET_COMPILE_DEFS "${Char_Flags};_CRT_SECURE_NO_DEPRECATE;_CRT_SECURE_NO_WARNINGS;PHTRANSLATE_STATICLIB;_BIND_TO_CURRENT_CRT_VERSION")

IF(CMAKE_COMPILER_IS_GNUCC)
  SET(TARGET_COMPILE_FLAGS "-std=gnu++11") # Add C++11 support for GCC
ENDIF()

IF(MSVC)
  SET(TARGET_COMPILE_FLAGS "${TARGET_COMPILE_FLAGS} /Zc:wchar_t- /Zc:forScope")
ENDIF(MSVC)

IF(CFUGUE_VERBOSE_TRACE)
	SET(TARGET_COMPILE_DEFS "${TARGET_COMPILE_DEFS};ENABLE_TRACING")
ENDIF()

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	FIND_PACKAGE(ALSA REQUIRED)
	SET(TARGET_COMPILE_DEFS "${TARGET_COMPILE_DEFS};__LINUX_ALSASEQ__")
	INCLUDE_DIRECTORIES (ALSA_INCLUDE_DIRS)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

IF(APPLE)	# Mac OS X specific code
	SET(TARGET_COMPILE_DEFS "${TARGET_COMPILE_DEFS};__MACOSX_CORE__")
	INCLUDE_DIRECTORIES ( /Developer/Headers/FlatCarbon )
ENDIF(APPLE)

IF(WIN32)	# Windows specific definitions
	SET(TARGET_COMPILE_DEFS "${TARGET_COMPILE_DEFS};__WINDOWS_MM__")
ENDIF(WIN32)

#################################

include_directories( "include" "include/common" )

#################################
#### Target: CFugueLib ####
#################################
SET( CFugueLib_Source_Files 
	src/CFugueLib/CFugueLib.cpp
	src/CFugueLib/Chords.cpp
	src/CFugueLib/Dictionary.cpp
	src/CFugueLib/Documentation.cpp
	src/CFugueLib/Instrument.cpp
	src/CFugueLib/MidiRenderer.cpp
	src/CFugueLib/MusicStringParser.cpp
	src/CFugueLib/Parser.cpp
	src/CFugueLib/Player.cpp	
	src/CFugueLib/stdafx.cpp
	src/CFugueLib/MIDIDrivers/AlsaDriver.cpp
	src/CFugueLib/MIDIDrivers/RtMidi.cpp
	src/CFugueLib/MIDIDrivers/MidiDevice.cpp
   )
SET( CFugueLib_Header_Files 
	src/CFugueLib/targetver.h
	src/CFugueLib/stdafx.h
	include/Common/EventHandler.h
	include/Common/MString.h
	include/rtmidi/RtMidi.h
	include/rtmidi/RtError.h
	include/MidiDevice.h
	include/AlsaDriver.h
	include/MidiTimer.h
	include/ControllerEvent.h
	include/Dictionary.h
	include/Instrument.h
	include/KeySignature.h
	include/Layer.h
	include/MidiEventManager.h
	include/MidiRenderer.h
	include/CFugueLib.h
	include/MusicStringParser.h
	include/Note.h
	include/Parser.h
	include/ParserListener.h
	include/Player.h
	include/Talam.h
	include/Tempo.h
	include/TimeToken.h
	include/Voice.h
   )
SET( CFugueLib_UnKnownGroup src/CFugueLib/ReadMe.txt    )

add_library(CFugue  ${CFugueLib_Source_Files}  ${CFugueLib_Header_Files}  ${CFugueLib_UnKnownGroup} )
set_target_properties(CFugue PROPERTIES COMPILE_DEFINITIONS "${TARGET_COMPILE_DEFS}" COMPILE_FLAGS "${TARGET_COMPILE_FLAGS}")

#################################
#### Target: Documentation ####
#################################
#################################
#### Target: jdkmidi ####
#################################
SET( jdkmidi_Source_Files 
	src/3rdparty/libjdkmidi/src/jdkmidi_advancedsequencer.cpp
	src/3rdparty/libjdkmidi/src/jdkmidi_driver.cpp
	src/3rdparty/libjdkmidi/src/jdkmidi_driverdump.cpp
	src/3rdparty/libjdkmidi/src/win32/jdkmidi_driverwin32.cpp
	src/3rdparty/libjdkmidi/src/jdkmidi_edittrack.cpp
	src/3rdparty/libjdkmidi/src/jdkmidi_file.cpp
	src/3rdparty/libjdkmidi/src/jdkmidi_fileread.cpp
	src/3rdparty/libjdkmidi/src/jdkmidi_filereadmultitrack.cpp
	src/3rdparty/libjdkmidi/src/jdkmidi_fileshow.cpp
	src/3rdparty/libjdkmidi/src/jdkmidi_filewrite.cpp
	src/3rdparty/libjdkmidi/src/jdkmidi_filewritemultitrack.cpp
	src/3rdparty/libjdkmidi/src/jdkmidi_keysig.cpp
	src/3rdparty/libjdkmidi/src/jdkmidi_manager.cpp
	src/3rdparty/libjdkmidi/src/jdkmidi_matrix.cpp
	src/3rdparty/libjdkmidi/src/jdkmidi_midi.cpp
	src/3rdparty/libjdkmidi/src/jdkmidi_msg.cpp
	src/3rdparty/libjdkmidi/src/jdkmidi_multitrack.cpp
	src/3rdparty/libjdkmidi/src/jdkmidi_parser.cpp
	src/3rdparty/libjdkmidi/src/jdkmidi_process.cpp
	src/3rdparty/libjdkmidi/src/jdkmidi_queue.cpp
	src/3rdparty/libjdkmidi/src/jdkmidi_sequencer.cpp
	src/3rdparty/libjdkmidi/src/jdkmidi_showcontrol.cpp
	src/3rdparty/libjdkmidi/src/jdkmidi_showcontrolhandler.cpp
	src/3rdparty/libjdkmidi/src/jdkmidi_smpte.cpp
	src/3rdparty/libjdkmidi/src/jdkmidi_sysex.cpp
	src/3rdparty/libjdkmidi/src/jdkmidi_tempo.cpp
	src/3rdparty/libjdkmidi/src/jdkmidi_tick.cpp
	src/3rdparty/libjdkmidi/src/jdkmidi_track.cpp
   )
SET( jdkmidi_Header_Files 
	include/jdkmidi/advancedsequencer.h
	include/jdkmidi/driver.h
	include/jdkmidi/driverdump.h
	include/jdkmidi/driverwin32.h
	include/jdkmidi/edittrack.h
	include/jdkmidi/file.h
	include/jdkmidi/fileread.h
	include/jdkmidi/filereadmultitrack.h
	include/jdkmidi/fileshow.h
	include/jdkmidi/filewrite.h
	include/jdkmidi/filewritemultitrack.h
	include/jdkmidi/keysig.h
	include/jdkmidi/manager.h
	include/jdkmidi/matrix.h
	include/jdkmidi/midi.h
	include/jdkmidi/msg.h
	include/jdkmidi/multitrack.h
	include/jdkmidi/parser.h
	include/jdkmidi/process.h
	include/jdkmidi/queue.h
	include/jdkmidi/sequencer.h
	include/jdkmidi/showcontrol.h
	include/jdkmidi/showcontrolhandler.h
	include/jdkmidi/smpte.h
	include/jdkmidi/song.h
	include/jdkmidi/sysex.h
	include/jdkmidi/tempo.h
	include/jdkmidi/tick.h
	include/jdkmidi/track.h
	include/jdkmidi/world.h
   )

add_library(jdkmidi  ${jdkmidi_Source_Files}  ${jdkmidi_Header_Files} )
set_target_properties(jdkmidi PROPERTIES COMPILE_DEFINITIONS "${TARGET_COMPILE_DEFS}" COMPILE_FLAGS "${TARGET_COMPILE_FLAGS}")

#################################
#Link the targets with their Depenedency Libraries
#################################

IF(WIN32)
	SET( jdkmidi_Librarian   WinMM.Lib   )
ENDIF(WIN32)

IF(APPLE)
   FIND_LIBRARY(CARBON_LIBRARY Carbon)
   FIND_LIBRARY(QUICKTIME_LIBRARY /Developer/SDKs/MacOSX10.7.sdk/System/Library/Frameworks//QuickTime.framework/QuickTime)
   FIND_LIBRARY(APP_SERVICES_LIBRARY ApplicationServices )
   FIND_LIBRARY(COREAUDIO_LIBRARY CoreAudio)
   FIND_LIBRARY(COREMIDI_LIBRARY CoreMIDI)
   FIND_LIBRARY(CORESERVICES_LIBRARY CoreServices)
   FIND_LIBRARY(COREFOUNDATION_LIBRARY CoreFoundation )
   MARK_AS_ADVANCED (CARBON_LIBRARY
                     QUICKTIME_LIBRARY
                     APP_SERVICES_LIBRARY
                     COREAUDIO_LIBRARY
					 COREMIDI_LIBRARY
					 CORESERVICES_LIBRARY
                     COREFOUNDATION_LIBRARY)
   SET(CFugueLib_Librarian ${CARBON_LIBRARY} ${QUICKTIME_LIBRARY} ${APP_SERVICES_LIBRARY} ${COREAUDIO_LIBRARY} ${COREMIDI_LIBRARY} ${CORESERVICES_LIBRARY} ${COREFOUNDATION_LIBRARY})
ENDIF (APPLE)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")	# Linux specific code
     SET(CFugueLib_Librarian ${ALSA_LIBRARIES} pthread)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")


SET( jdkmidi_Dependencies  ${jdkmidi_Librarian} )
target_link_libraries(jdkmidi  ${jdkmidi_Dependencies})

SET( CFugueLib_Dependencies jdkmidi  ${CFugueLib_Librarian} )
target_link_libraries(CFugue  ${CFugueLib_Dependencies})

install(TARGETS jdkmidi CFugue RUNTIME DESTINATION bin  LIBRARY DESTINATION bin ARCHIVE DESTINATION lib)

#################################
#### Target: CFugueDLL ####
#################################

if (CFUGUE_BUILD_TESTS AND CFUGUE_BUILD_DLL AND WIN32)	
	ADD_SUBDIRECTORY(src/CFugueDLL)
ENDIF(CFUGUE_BUILD_TESTS AND CFUGUE_BUILD_DLL AND WIN32)


#################################
####         Tests           ####
#################################

if (CFUGUE_BUILD_TESTS)

	# Set path to additional CMake modules
	SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/")
	FIND_PACKAGE(Tse3)
	if(NOT TSE3_FOUND)
		if(WIN32)
			#Message(STATUS "Building TSE3 from source")
			#message(STATUS "Turning TSE3 Unicode ON")	
			#SET (TSE3_UNICODE_BUILD ON CACHE BOOL "CarMusTy requires Unicode to be ON" FORCE)		
			#ADD_SUBDIRECTORY(src/3rdparty/tse3)
			#SET (TSE3_LIBRARIES 	tse3)
			#SET (TSE3_INCLUDE_DIR 	"${CMAKE_CURRENT_SOURCE_DIR}/src/3rdparty/tse3/src")
		else()
			# We do NOT support building TSE3 from source on non-windows system
			Message(STATUS "To enable TSE3 support, install the package: libtse3-dev")
		endif()
	endif()
	
  ADD_SUBDIRECTORY(test)
  
endif(CFUGUE_BUILD_TESTS)

# MESSAGE(TODO:Add Doxygen Target for documentation)
