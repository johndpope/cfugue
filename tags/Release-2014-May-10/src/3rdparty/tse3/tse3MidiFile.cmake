###################################################
###  Auto-Generated with GPalem CMake Generator    
###                 from [Debug|Win32]
###  D:/WinApp/CarMusTy/src/References/CFugue/src/3rdparty/tse3/build/tse3MidiFile.vcxproj
###################################################

cmake_minimum_required(VERSION 2.8)

Project(tse3MidiFile)

Set(ProjDir ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(
	${ProjDir}/src/   )
link_directories(
	${ProjDir}/lib/   )

Set(tse3MidiFile_ClInclude_Files   )
Set(tse3MidiFile_ClCompile_Files
	${ProjDir}/src/examples/midifile/midifile.cpp   )
Set(tse3MidiFile_ResourceCompile_Files   )
Set(tse3MidiFile_Manifest_Files   )
Set(tse3MidiFile_Midl_Files   )
Set(tse3MidiFile_CustomBuild_Files   )

Set(RUNTIME_OUTPUT_DIRECTORY ${ProjDir}/bin)

Add_Executable(tse3MidiFile ${tse3MidiFile_ClInclude_Files} ${tse3MidiFile_ClCompile_Files} ${tse3MidiFile_ResourceCompile_Files} ${tse3MidiFile_Manifest_Files} ${tse3MidiFile_Midl_Files} ${tse3MidiFile_CustomBuild_Files})

set_target_properties(tse3MidiFile PROPERTIES 
	COMPILE_DEFINITIONS "${TARGET_COMPILE_DEFS}"
	COMPILE_FLAGS "${TARGET_COMPILE_FLAGS}")

Set(tse3MidiFile_Dependencies  tse3)
target_link_libraries(tse3MidiFile ${tse3MidiFile_Dependencies})

