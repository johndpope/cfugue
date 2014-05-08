###################################################
###  Auto-Generated with GPalem CMake Generator    
###                 from [Debug|Win32]
###  D:/WinApp/CarMusTy/src/References/CFugue/src/3rdparty/tse3/build/tse3.vcxproj
###################################################

cmake_minimum_required(VERSION 2.8)

Project(tse3)

Set(ProjDir ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(
	${ProjDir}/include
	${ProjDir}/src   )
link_directories(   )

Set(tse3_ClInclude_Files
	${ProjDir}/src/tse3/app/Application.h
	${ProjDir}/src/tse3/app/Choices.h
	${ProjDir}/src/tse3/app/Modified.h
	${ProjDir}/src/tse3/app/PartDisplay.h
	${ProjDir}/src/tse3/app/PartSelection.h
	${ProjDir}/src/tse3/app/Record.h
	${ProjDir}/src/tse3/app/TrackSelection.h
	${ProjDir}/src/tse3/DisplayParams.h
	${ProjDir}/src/tse3/Error.h
	${ProjDir}/src/tse3/EventTrack.h
	${ProjDir}/src/tse3/FileBlockParser.h
	${ProjDir}/src/tse3/Filter.h
	${ProjDir}/src/tse3/FlagTrack.h
	${ProjDir}/src/tse3/KeySigTrack.h
	${ProjDir}/src/tse3/Metronome.h
	${ProjDir}/src/tse3/Midi.h
	${ProjDir}/src/tse3/MidiCommandFilter.h
	${ProjDir}/src/tse3/MidiData.h
	${ProjDir}/src/tse3/MidiEcho.h
	${ProjDir}/src/tse3/MidiFile.h
	${ProjDir}/src/tse3/MidiFilter.h
	${ProjDir}/src/tse3/MidiMapper.h
	${ProjDir}/src/tse3/MidiParams.h
	${ProjDir}/src/tse3/MidiScheduler.h
	${ProjDir}/src/tse3/Mixer.h
	${ProjDir}/src/tse3/Mutex.h
	${ProjDir}/src/tse3/Notifier.h
	${ProjDir}/src/tse3/Panic.h
	${ProjDir}/src/tse3/Part.h
	${ProjDir}/src/tse3/Phrase.h
	${ProjDir}/src/tse3/PhraseEdit.h
	${ProjDir}/src/tse3/PhraseList.h
	${ProjDir}/src/tse3/Playable.h
	${ProjDir}/src/tse3/Progress.h
	${ProjDir}/src/tse3/RepeatTrack.h
	${ProjDir}/src/tse3/Serializable.h
	${ProjDir}/src/tse3/Song.h
	${ProjDir}/src/tse3/TempoTrack.h
	${ProjDir}/src/tse3/TimeSigTrack.h
	${ProjDir}/src/tse3/Track.h
	${ProjDir}/src/tse3/Transport.h
	${ProjDir}/src/tse3/TSE2MDL.h
	${ProjDir}/src/tse3/TSE3.h
	${ProjDir}/src/tse3/TSE3MDL.h
	${ProjDir}/src/tse3/plt/Factory.h
	${ProjDir}/src/tse3/plt/midiswis.h
	${ProjDir}/src/tse3/util/Demidify.h
	${ProjDir}/src/tse3/util/MidiScheduler.h
	${ProjDir}/src/tse3/util/MulDiv.h
	${ProjDir}/src/tse3/util/NoteNumber.h
	${ProjDir}/src/tse3/util/Phrase.h
	${ProjDir}/src/tse3/util/PowerQuantise.h
	${ProjDir}/src/tse3/util/Snap.h
	${ProjDir}/src/tse3/util/Song.h
	${ProjDir}/src/tse3/util/Track.h   )
Set(tse3_ClCompile_Files
	${ProjDir}/src/tse3/app/Application.cpp
	${ProjDir}/src/tse3/app/Choices.cpp
	${ProjDir}/src/tse3/app/Modified.cpp
	${ProjDir}/src/tse3/app/PartDisplay.cpp
	${ProjDir}/src/tse3/app/PartSelection.cpp
	${ProjDir}/src/tse3/app/Record.cpp
	${ProjDir}/src/tse3/app/TrackSelection.cpp
	${ProjDir}/src/tse3/DisplayParams.cpp
	${ProjDir}/src/tse3/Error.cpp
	${ProjDir}/src/tse3/FileBlockParser.cpp
	${ProjDir}/src/tse3/Filter.cpp
	${ProjDir}/src/tse3/FlagTrack.cpp
	${ProjDir}/src/tse3/KeySigTrack.cpp
	${ProjDir}/src/tse3/Metronome.cpp
	${ProjDir}/src/tse3/Midi.cpp
	${ProjDir}/src/tse3/MidiCommandFilter.cpp
	${ProjDir}/src/tse3/MidiData.cpp
	${ProjDir}/src/tse3/MidiEcho.cpp
	${ProjDir}/src/tse3/MidiFile.cpp
	${ProjDir}/src/tse3/MidiFilter.cpp
	${ProjDir}/src/tse3/MidiMapper.cpp
	${ProjDir}/src/tse3/MidiParams.cpp
	${ProjDir}/src/tse3/MidiScheduler.cpp
	${ProjDir}/src/tse3/Mixer.cpp
	${ProjDir}/src/tse3/Mutex.cpp
	${ProjDir}/src/tse3/Notifier.cpp
	${ProjDir}/src/tse3/Panic.cpp
	${ProjDir}/src/tse3/Part.cpp
	${ProjDir}/src/tse3/Phrase.cpp
	${ProjDir}/src/tse3/PhraseEdit.cpp
	${ProjDir}/src/tse3/PhraseList.cpp
	${ProjDir}/src/tse3/Playable.cpp
	${ProjDir}/src/tse3/RepeatTrack.cpp
	${ProjDir}/src/tse3/Serializable.cpp
	${ProjDir}/src/tse3/Song.cpp
	${ProjDir}/src/tse3/TempoTrack.cpp
	${ProjDir}/src/tse3/test.cpp
	${ProjDir}/src/tse3/TimeSigTrack.cpp
	${ProjDir}/src/tse3/Track.cpp
	${ProjDir}/src/tse3/Transport.cpp
	${ProjDir}/src/tse3/TSE2MDL.cpp
	${ProjDir}/src/tse3/TSE3.cpp
	${ProjDir}/src/tse3/TSE3MDL.cpp
	${ProjDir}/src/tse3/util/Demidify.cpp
	${ProjDir}/src/tse3/util/MidiScheduler.cpp
	${ProjDir}/src/tse3/util/MulDiv.cpp
	${ProjDir}/src/tse3/util/NoteNumber.cpp
	${ProjDir}/src/tse3/util/Phrase.cpp
	${ProjDir}/src/tse3/util/PowerQuantise.cpp
	${ProjDir}/src/tse3/util/Snap.cpp
	${ProjDir}/src/tse3/util/Song.cpp
	${ProjDir}/src/tse3/util/Track.cpp   )
Set(tse3_ResourceCompile_Files   )
Set(tse3_Manifest_Files   )
Set(tse3_Midl_Files   )
Set(tse3_CustomBuild_Files   )

if(WIN32)
	Set(tse3_ClCompile_Files ${tse3_ClCompile_Files} ${ProjDir}/src/tse3/plt/Win32.cpp)
	Set(tse3_ClInclude_Files ${tse3_ClInclude_Files} ${ProjDir}/src/tse3/plt/Win32.h)
	Set(tse3_Plt_Dependencies WinMM.lib)
else()
	Message(FATAL_ERROR "TSE3: Build from source is not supported on this platform. Try installing libtse3-dev package")
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")	# Linux specific code
	# Its not worth doing the below for us. Linux supports libtse3-dev package, get it
	Set(TARGET_COMPILE_DEFS "HAVE_ALSA_ASOUNDLIB_H=1;${TARGET_COMPILE_DEFS};")
	Set(tse3_ClCompile_Files ${tse3_ClCompile_Files} 
							${ProjDir}/src/tse3/plt/Alsa-0.5.cpp
							${ProjDir}/src/tse3/plt/Arts.cpp
							${ProjDir}/src/tse3/plt/Factory_Unix.cpp
							${ProjDir}/src/tse3/plt/OSS.cpp)
	Set(tse3_ClInclude_Files ${tse3_ClInclude_Files} 
							${ProjDir}/src/tse3/plt/Alsa.h
							${ProjDir}/src/tse3/plt/Arts.h
							${ProjDir}/src/tse3/plt/Factory.h
							${ProjDir}/src/tse3/plt/OSS.h)
endif()

Add_Library(tse3 STATIC  ${tse3_ClInclude_Files} ${tse3_ClCompile_Files} ${tse3_ResourceCompile_Files} ${tse3_Manifest_Files} ${tse3_Midl_Files} ${tse3_CustomBuild_Files})

set_target_properties(tse3 PROPERTIES 
	COMPILE_DEFINITIONS "${TARGET_COMPILE_DEFS}"
	COMPILE_FLAGS "${TARGET_COMPILE_FLAGS}")

Set(tse3_Dependencies ${tse3_Plt_Dependencies} ins cmd tse3file)
target_link_libraries(tse3 ${tse3_Dependencies})

install(TARGETS tse3 RUNTIME DESTINATION bin  LIBRARY DESTINATION bin ARCHIVE DESTINATION lib)