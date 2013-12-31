###################################################
###  Auto-Generated with GPalem CMake Generator    
###                 from [Debug|Win32]
###  D:/WinApp/CarMusTy/src/References/CFugue/src/3rdparty/tse3/build/file.vcxproj
###################################################

cmake_minimum_required(VERSION 2.8)

Project(tse3file)

Set(ProjDir ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(
	${ProjDir}/src/   )
link_directories(   )

Set(file_ClInclude_Files
	${ProjDir}/src/tse3/file/Write.h
	${ProjDir}/src/tse3/file/XML.h   )
Set(file_ClCompile_Files
	${ProjDir}/src/tse3/file/DisplayParams.cpp
	${ProjDir}/src/tse3/file/FlagTrack.cpp
	${ProjDir}/src/tse3/file/KeySigTrack.cpp
	${ProjDir}/src/tse3/file/MidiFilter.cpp
	${ProjDir}/src/tse3/file/MidiParams.cpp
	${ProjDir}/src/tse3/file/Part.cpp
	${ProjDir}/src/tse3/file/PhraseList.cpp
	${ProjDir}/src/tse3/file/Song.cpp
	${ProjDir}/src/tse3/file/TempoTrack.cpp
	${ProjDir}/src/tse3/file/TimeSigTrack.cpp
	${ProjDir}/src/tse3/file/Track.cpp
	${ProjDir}/src/tse3/file/Write.cpp
	${ProjDir}/src/tse3/file/XML.cpp   )
Set(file_ResourceCompile_Files   )
Set(file_Manifest_Files   )
Set(file_Midl_Files   )
Set(file_CustomBuild_Files   )

Add_Library(tse3file STATIC  ${file_ClInclude_Files} ${file_ClCompile_Files} ${file_ResourceCompile_Files} ${file_Manifest_Files} ${file_Midl_Files} ${file_CustomBuild_Files})

set_target_properties(tse3file PROPERTIES 
	COMPILE_DEFINITIONS "${TARGET_COMPILE_DEFS}"
	COMPILE_FLAGS "${TARGET_COMPILE_FLAGS}")

Set(file_Dependencies  )
target_link_libraries(tse3file ${file_Dependencies})

