###################################################
###  Auto-Generated with GPalem CMake Generator    
###                 from [Debug|Win32]
###  D:/WinApp/CarMusTy/src/References/CFugue/src/3rdparty/tse3/build/listen.vcxproj
###################################################

cmake_minimum_required(VERSION 2.8)

Project(listen)

Set(ProjDir ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(
	${ProjDir}/src/   )
link_directories(   )

Set(listen_ClInclude_Files
	${ProjDir}/src/tse3/listen/app/Modified.h
	${ProjDir}/src/tse3/listen/app/PartSelection.h
	${ProjDir}/src/tse3/listen/app/Record.h
	${ProjDir}/src/tse3/listen/app/TrackSelection.h
	${ProjDir}/src/tse3/listen/cmd/CommandHistory.h
	${ProjDir}/src/tse3/listen/DisplayParams.h
	${ProjDir}/src/tse3/listen/EventTrack.h
	${ProjDir}/src/tse3/listen/FlagTrack.h
	${ProjDir}/src/tse3/listen/ins/Destination.h
	${ProjDir}/src/tse3/listen/KeySigTrack.h
	${ProjDir}/src/tse3/listen/Metronome.h
	${ProjDir}/src/tse3/listen/MidiCommandFilter.h
	${ProjDir}/src/tse3/listen/MidiData.h
	${ProjDir}/src/tse3/listen/MidiEcho.h
	${ProjDir}/src/tse3/listen/MidiFile.h
	${ProjDir}/src/tse3/listen/MidiFilter.h
	${ProjDir}/src/tse3/listen/MidiMapper.h
	${ProjDir}/src/tse3/listen/MidiParams.h
	${ProjDir}/src/tse3/listen/MidiScheduler.h
	${ProjDir}/src/tse3/listen/Mixer.h
	${ProjDir}/src/tse3/listen/Panic.h
	${ProjDir}/src/tse3/listen/Part.h
	${ProjDir}/src/tse3/listen/Phrase.h
	${ProjDir}/src/tse3/listen/PhraseEdit.h
	${ProjDir}/src/tse3/listen/PhraseList.h
	${ProjDir}/src/tse3/listen/Playable.h
	${ProjDir}/src/tse3/listen/RepeatTrack.h
	${ProjDir}/src/tse3/listen/Song.h
	${ProjDir}/src/tse3/listen/TempoTrack.h
	${ProjDir}/src/tse3/listen/TimeSigTrack.h
	${ProjDir}/src/tse3/listen/Track.h
	${ProjDir}/src/tse3/listen/Transport.h   )
Set(listen_ClCompile_Files   )
Set(listen_ResourceCompile_Files   )
Set(listen_Manifest_Files   )
Set(listen_Midl_Files   )
Set(listen_CustomBuild_Files   )

Add_Library(listen STATIC  ${listen_ClInclude_Files} ${listen_ClCompile_Files} ${listen_ResourceCompile_Files} ${listen_Manifest_Files} ${listen_Midl_Files} ${listen_CustomBuild_Files})

set_target_properties(listen PROPERTIES 
	COMPILE_DEFINITIONS "${TARGET_COMPILE_DEFS}"
	COMPILE_FLAGS "${TARGET_COMPILE_FLAGS}")

Set(listen_Dependencies  )
target_link_libraries(listen ${listen_Dependencies})

