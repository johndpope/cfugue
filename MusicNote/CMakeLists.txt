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
option (BUILD_CFUGUE_DLL "Should the DLL be built for CFugue (Only for Windows)" OFF) 
option (BUILD_CFUGUE_TESTS "Should the Test applications be built for CFugue" ON) 
OPTION (VERBOSE_CFUGUE_TRACE "Should CFugue generate Verbose String Parsing Traces" ON)
SET (BUILD_CFUGUE_UNICODE ON CACHE BOOL "Should CFugue be built with Unicode? (recommended: Yes)")

mark_as_advanced(BUILD_CFUGUE_DLL)

#################################
# Compute Values based on Options
#################################
set(Char_Flags "" CACHE INTERNAL "Compiler Character Flags: Unicode/MBCS Options")
set(Char_postfix "" CACHE INTERNAL "Unicode Postfix")
set(Output_postfix "" CACHE INTERNAL "Debug Postfix")

if(BUILD_CFUGUE_UNICODE)
	SET(Char_postfix "u")
	SET(Char_Flags "-DUNICODE -D_UNICODE")
	message(STATUS "CFugue: Unicode build is ON")
else(BUILD_CFUGUE_UNICODE)
	SET(Char_postfix "")
	SET(Char_Flags "-D_MBCS")
	message(STATUS "CFugue: Unicode build is OFF")
endif(BUILD_CFUGUE_UNICODE)

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

ADD_DEFINITIONS(${Char_Flags} -D_CRT_SECURE_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS)

IF(CMAKE_COMPILER_IS_GNUCC)
  ADD_DEFINITIONS(-std=gnu++0x ) # Add C++0x support for GCC
ENDIF()

IF(VERBOSE_CFUGUE_TRACE)
	ADD_DEFINITIONS(-DENABLE_TRACING)
ENDIF()

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	ADD_DEFINITIONS(-D__LINUX_ALSASEQ__)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

IF(APPLE)	# Mac OS X specific code
	ADD_DEFINITIONS(-D__MACOSX_CORE__)
	INCLUDE_DIRECTORIES ( /Developer/Headers/FlatCarbon )
ENDIF(APPLE)

IF(WIN32)	# Windows specific definitions
	ADD_DEFINITIONS(-D__WINDOWS_MM__)
ENDIF(WIN32)

#################################

include_directories( "include" "include/common" )

#################################
#### Target: CFugueLib ####
#################################
SET( MusicNoteLib_Source_Files 
	src/MusicNoteLib/Chords.cpp
	src/MusicNoteLib/Dictionary.cpp
	src/MusicNoteLib/Documentation.cpp
	src/MusicNoteLib/Instrument.cpp
	src/MusicNoteLib/MIDIRenderer.cpp
	src/MusicNoteLib/MusicNoteLib.cpp
	src/MusicNoteLib/MusicStringParser.cpp
	src/MusicNoteLib/Parser.cpp
	src/MusicNoteLib/Player.cpp	
	src/MusicNoteLib/stdafx.cpp
	src/MusicNoteLib/MIDIDrivers/AlsaDriver.cpp
	src/MusicNoteLib/MIDIDrivers/RtMidi.cpp
	src/MusicNoteLib/MIDIDrivers/MidiDevice.cpp
   )
SET( MusicNoteLib_Header_Files 
	src/MusicNoteLib/targetver.h
	src/MusicNoteLib/stdafx.h
	include/Common/EventHandler.h
	include/Common/StrUtils.h
	include/Common/MString.h
	include/rtmidi/RtMidi.h
	include/rtmidi/RtError.h
	include/MidiDevice.h
	include/AlsaDriver.h
	include/MidiTimer.h
	include/Dictionary.h
	include/Instrument.h
	include/KeySignature.h
	include/Layer.h
	include/MIDIEventManager.h
	include/MIDIRenderer.h
	include/MusicNoteLib.h
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
SET( MusicNoteLib_UnKnownGroup src/MusicNoteLib/ReadMe.txt    )

add_library(libCFugue  ${MusicNoteLib_Source_Files}  ${MusicNoteLib_Header_Files}  ${MusicNoteLib_UnKnownGroup} )

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

add_library(jdkmidi   ${jdkmidi_Source_Files}  ${jdkmidi_Header_Files} )

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
   SET(MusicNoteLib_Librarian ${CARBON_LIBRARY} ${QUICKTIME_LIBRARY} ${APP_SERVICES_LIBRARY} ${COREAUDIO_LIBRARY} ${COREMIDI_LIBRARY} ${CORESERVICES_LIBRARY} ${COREFOUNDATION_LIBRARY})
ENDIF (APPLE)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")	# Linux specific code
     SET(MusicNoteLib_Librarian asound)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")


SET( jdkmidi_Dependencies  ${jdkmidi_Librarian} )
target_link_libraries(jdkmidi  ${jdkmidi_Dependencies})

SET( MusicNoteLib_Dependencies jdkmidi  ${MusicNoteLib_Librarian} )
target_link_libraries(libCFugue  ${MusicNoteLib_Dependencies})

install(TARGETS jdkmidi libCFugue RUNTIME DESTINATION bin  LIBRARY DESTINATION bin ARCHIVE DESTINATION lib)

#################################
#### Target: CFugueDLL ####
#################################

if (BUILD_CFUGUE_TESTS AND BUILD_CFUGUE_DLL AND WIN32)	
	ADD_SUBDIRECTORY(src/MusicNoteDLL)
ENDIF(BUILD_CFUGUE_TESTS AND BUILD_CFUGUE_DLL AND WIN32)


#################################
####         Tests           ####
#################################

if (BUILD_CFUGUE_TESTS)

	# Set path to additional CMake modules
	SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/")
	FIND_PACKAGE(Tse3)
	if(NOT TSE3_FOUND)
		Message(STATUS "Building TSE3 from source")
		ADD_SUBDIRECTORY(src/3rdparty/tse3)
	endif()
	
  ADD_SUBDIRECTORY(test)
  
endif(BUILD_CFUGUE_TESTS)

# MESSAGE(TODO:Add Doxygen Target for documentation)
