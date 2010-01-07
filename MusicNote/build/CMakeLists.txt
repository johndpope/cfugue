# CMakeLists.txt - Created with Sln2CMake Application


cmake_minimum_required(VERSION 2.6)

PROJECT( MusicNote )

#################################
#### Target: MusicNoteLib ####
#################################
include_directories( "../include" "../../common" )
SET( MusicNoteLib_Source_Files 
	../src/MusicNoteLib/Dictionary.cpp
	../src/MusicNoteLib/Instrument.cpp
	../src/MusicNoteLib/MIDIRenderer.cpp
	../src/MusicNoteLib/MusicNoteLib.cpp
	../src/MusicNoteLib/MusicStringParser.cpp
	../src/MusicNoteLib/Parser.cpp
	../src/MusicNoteLib/stdafx.cpp
   )
SET( MusicNoteLib_Header_Files 
	../include/Dictionary.h
	../../Common/EventHandler.h
	../include/Instrument.h
	../include/KeySignature.h
	../include/Layer.h
	../include/MIDIEventManager.h
	../include/MIDIRenderer.h
	../include/MString.h
	../include/MusicNoteLib.h
	../include/MusicStringParser.h
	../include/Note.h
	../include/Parser.h
	../include/ParserListener.h
	../include/Player.h
	../src/MusicNoteLib/stdafx.h
	../include/Talam.h
	../src/MusicNoteLib/targetver.h
	../include/Tempo.h
	../include/Time.h
	../include/Voice.h
   )
SET( MusicNoteLib_UnKnownGroup ../src/MusicNoteLib/ReadMe.txt    )

add_library(MusicNoteLib   ${MusicNoteLib_Source_Files}  ${MusicNoteLib_Header_Files}  ${MusicNoteLib_UnKnownGroup} )

#################################
#### Target: ConsoleTestApp ####
#################################
include_directories( "../include" "../../Common" )
link_directories( "../lib" )
SET( ConsoleTestApp_Source_Files 
	../test/ConsoleTestApp/ConsoleTestApp.cpp
	../test/ConsoleTestApp/stdafx.cpp
   )
SET( ConsoleTestApp_Header_Files 
	../test/ConsoleTestApp/stdafx.h
	../test/ConsoleTestApp/targetver.h
   )
SET( ConsoleTestApp_UnKnownGroup ../test/ConsoleTestApp/ReadMe.txt    )

add_executable(ConsoleTestApp   ${ConsoleTestApp_Source_Files}  ${ConsoleTestApp_Header_Files}  ${ConsoleTestApp_UnKnownGroup} )

#################################
#### Target: MusicNoteDLL ####
#################################
include_directories( "../include" "../../common" )
SET( MusicNoteDLL_Source_Files 
	../src/MusicNoteDLL/dlldatax.c
	../src/MusicNoteDLL/dllmain.cpp
	../src/MusicNoteDLL/MusicNoteDLL.cpp
	../src/MusicNoteDLL/MusicNoteDLL.def
	../src/MusicNoteDLL/MusicNoteDLL.idl
	../src/MusicNoteLib/MusicNoteLib.cpp
	../src/MusicNoteDLL/stdafx.cpp
   )
SET( MusicNoteDLL_Header_Files 
	../src/MusicNoteDLL/dlldatax.h
	../src/MusicNoteDLL/dllmain.h
	../include/MusicNoteDll.h
	../src/MusicNoteDLL/Resource.h
	../src/MusicNoteDLL/stdafx.h
	../src/MusicNoteDLL/targetver.h
   )
SET( MusicNoteDLL_Resource_Files 
	#../src/MusicNoteDLL/MusicNoteDLL.rc
	../src/MusicNoteDLL/MusicNoteDLL.rgs
   )
SET( MusicNoteDLL_Generated_Files 
	../src/MusicNoteDLL/MusicNoteDLL_i.c
	../src/MusicNoteDLL/MusicNoteDLL_i.h
   )
SET( MusicNoteDLL_UnKnownGroup ../src/MusicNoteDLL/ReadMe.txt    )

add_library(MusicNoteDLL  SHARED   ${MusicNoteDLL_Source_Files}  ${MusicNoteDLL_Header_Files}  ${MusicNoteDLL_Resource_Files}  ${MusicNoteDLL_Generated_Files}  ${MusicNoteDLL_UnKnownGroup} )

#################################
#### Target: PlayerTestApp ####
#################################
#################################
#### Target: Documentation ####
#################################
#################################
#### Target: jdkmidi ####
#################################
include_directories( "../src/3rdparty/libjdkmidi/build/vc9/../../../../../include" )
SET( jdkmidi_Librarian   WinMM.Lib   )
SET( jdkmidi_Source_Files 
	../src/3rdparty/libjdkmidi/build/vc9/../../src/jdkmidi_advancedsequencer.cpp
	../src/3rdparty/libjdkmidi/build/vc9/../../src/jdkmidi_driver.cpp
	../src/3rdparty/libjdkmidi/build/vc9/../../src/jdkmidi_driverdump.cpp
	../src/3rdparty/libjdkmidi/build/vc9/../../src/win32/jdkmidi_driverwin32.cpp
	../src/3rdparty/libjdkmidi/build/vc9/../../src/jdkmidi_edittrack.cpp
	../src/3rdparty/libjdkmidi/build/vc9/../../src/jdkmidi_file.cpp
	../src/3rdparty/libjdkmidi/build/vc9/../../src/jdkmidi_fileread.cpp
	../src/3rdparty/libjdkmidi/build/vc9/../../src/jdkmidi_filereadmultitrack.cpp
	../src/3rdparty/libjdkmidi/build/vc9/../../src/jdkmidi_fileshow.cpp
	../src/3rdparty/libjdkmidi/build/vc9/../../src/jdkmidi_filewrite.cpp
	../src/3rdparty/libjdkmidi/build/vc9/../../src/jdkmidi_filewritemultitrack.cpp
	../src/3rdparty/libjdkmidi/build/vc9/../../src/jdkmidi_keysig.cpp
	../src/3rdparty/libjdkmidi/build/vc9/../../src/jdkmidi_manager.cpp
	../src/3rdparty/libjdkmidi/build/vc9/../../src/jdkmidi_matrix.cpp
	../src/3rdparty/libjdkmidi/build/vc9/../../src/jdkmidi_midi.cpp
	../src/3rdparty/libjdkmidi/build/vc9/../../src/jdkmidi_msg.cpp
	../src/3rdparty/libjdkmidi/build/vc9/../../src/jdkmidi_multitrack.cpp
	../src/3rdparty/libjdkmidi/build/vc9/../../src/jdkmidi_parser.cpp
	../src/3rdparty/libjdkmidi/build/vc9/../../src/jdkmidi_process.cpp
	../src/3rdparty/libjdkmidi/build/vc9/../../src/jdkmidi_queue.cpp
	../src/3rdparty/libjdkmidi/build/vc9/../../src/jdkmidi_sequencer.cpp
	../src/3rdparty/libjdkmidi/build/vc9/../../src/jdkmidi_showcontrol.cpp
	../src/3rdparty/libjdkmidi/build/vc9/../../src/jdkmidi_showcontrolhandler.cpp
	../src/3rdparty/libjdkmidi/build/vc9/../../src/jdkmidi_smpte.cpp
	../src/3rdparty/libjdkmidi/build/vc9/../../src/jdkmidi_sysex.cpp
	../src/3rdparty/libjdkmidi/build/vc9/../../src/jdkmidi_tempo.cpp
	../src/3rdparty/libjdkmidi/build/vc9/../../src/jdkmidi_tick.cpp
	../src/3rdparty/libjdkmidi/build/vc9/../../src/jdkmidi_track.cpp
   )
SET( jdkmidi_Header_Files 
	../src/3rdparty/libjdkmidi/build/vc9/../../../../../include/jdkmidi/advancedsequencer.h
	../src/3rdparty/libjdkmidi/build/vc9/../../../../../include/jdkmidi/driver.h
	../src/3rdparty/libjdkmidi/build/vc9/../../../../../include/jdkmidi/driverdump.h
	../src/3rdparty/libjdkmidi/build/vc9/../../../../../include/jdkmidi/driverwin32.h
	../src/3rdparty/libjdkmidi/build/vc9/../../../../../include/jdkmidi/edittrack.h
	../src/3rdparty/libjdkmidi/build/vc9/../../../../../include/jdkmidi/file.h
	../src/3rdparty/libjdkmidi/build/vc9/../../../../../include/jdkmidi/fileread.h
	../src/3rdparty/libjdkmidi/build/vc9/../../../../../include/jdkmidi/filereadmultitrack.h
	../src/3rdparty/libjdkmidi/build/vc9/../../../../../include/jdkmidi/fileshow.h
	../src/3rdparty/libjdkmidi/build/vc9/../../../../../include/jdkmidi/filewrite.h
	../src/3rdparty/libjdkmidi/build/vc9/../../../../../include/jdkmidi/filewritemultitrack.h
	../src/3rdparty/libjdkmidi/build/vc9/../../../../../include/jdkmidi/keysig.h
	../src/3rdparty/libjdkmidi/build/vc9/../../../../../include/jdkmidi/manager.h
	../src/3rdparty/libjdkmidi/build/vc9/../../../../../include/jdkmidi/matrix.h
	../src/3rdparty/libjdkmidi/build/vc9/../../../../../include/jdkmidi/midi.h
	../src/3rdparty/libjdkmidi/build/vc9/../../../../../include/jdkmidi/msg.h
	../src/3rdparty/libjdkmidi/build/vc9/../../../../../include/jdkmidi/multitrack.h
	../src/3rdparty/libjdkmidi/build/vc9/../../../../../include/jdkmidi/parser.h
	../src/3rdparty/libjdkmidi/build/vc9/../../../../../include/jdkmidi/process.h
	../src/3rdparty/libjdkmidi/build/vc9/../../../../../include/jdkmidi/queue.h
	../src/3rdparty/libjdkmidi/build/vc9/../../../../../include/jdkmidi/sequencer.h
	../src/3rdparty/libjdkmidi/build/vc9/../../../../../include/jdkmidi/showcontrol.h
	../src/3rdparty/libjdkmidi/build/vc9/../../../../../include/jdkmidi/showcontrolhandler.h
	../src/3rdparty/libjdkmidi/build/vc9/../../../../../include/jdkmidi/smpte.h
	../src/3rdparty/libjdkmidi/build/vc9/../../../../../include/jdkmidi/song.h
	../src/3rdparty/libjdkmidi/build/vc9/../../../../../include/jdkmidi/sysex.h
	../src/3rdparty/libjdkmidi/build/vc9/../../../../../include/jdkmidi/tempo.h
	../src/3rdparty/libjdkmidi/build/vc9/../../../../../include/jdkmidi/tick.h
	../src/3rdparty/libjdkmidi/build/vc9/../../../../../include/jdkmidi/track.h
	../src/3rdparty/libjdkmidi/build/vc9/../../../../../include/jdkmidi/world.h
   )

add_library(jdkmidi   ${jdkmidi_Source_Files}  ${jdkmidi_Header_Files} )

#################################
#Link the targets with their Depenedency Libraries
#################################
SET( MusicNoteLib_Dependencies jdkmidi  ${MusicNoteLib_Librarian} )
target_link_libraries(MusicNoteLib  ${MusicNoteLib_Dependencies})

SET( ConsoleTestApp_Dependencies MusicNoteLib  ${ConsoleTestApp_Librarian} )
target_link_libraries(ConsoleTestApp  ${ConsoleTestApp_Dependencies})

SET( MusicNoteDLL_Dependencies MusicNoteLib  ${MusicNoteDLL_Librarian} )
target_link_libraries(MusicNoteDLL  ${MusicNoteDLL_Dependencies})

SET( jdkmidi_Dependencies  ${jdkmidi_Librarian} )
target_link_libraries(jdkmidi  ${jdkmidi_Dependencies})

