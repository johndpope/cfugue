###################################################
###  Auto-Generated with GPalem CMake Generator    
###                 from [Debug|Win32]
###  D:/WinApp/CarMusTy/src/References/CFugue/src/3rdparty/tse3/build/cmd.vcxproj
###################################################

cmake_minimum_required(VERSION 2.8)

Project(cmd)

Set(ProjDir ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(
	${ProjDir}/src/   )
link_directories(   )

Set(cmd_ClInclude_Files
	${ProjDir}/src/tse3/cmd/Command.h
	${ProjDir}/src/tse3/cmd/CommandGroup.h
	${ProjDir}/src/tse3/cmd/CommandHistory.h
	${ProjDir}/src/tse3/cmd/FlagTrack.h
	${ProjDir}/src/tse3/cmd/Part.h
	${ProjDir}/src/tse3/cmd/Phrase.h
	${ProjDir}/src/tse3/cmd/Song.h
	${ProjDir}/src/tse3/cmd/Track.h   )
Set(cmd_ClCompile_Files
	${ProjDir}/src/tse3/cmd/CommandGroup.cpp
	${ProjDir}/src/tse3/cmd/CommandHistory.cpp
	${ProjDir}/src/tse3/cmd/FlagTrack.cpp
	${ProjDir}/src/tse3/cmd/Part.cpp
	${ProjDir}/src/tse3/cmd/Phrase.cpp
	${ProjDir}/src/tse3/cmd/Song.cpp
	${ProjDir}/src/tse3/cmd/Track.cpp   )
Set(cmd_ResourceCompile_Files   )
Set(cmd_Manifest_Files   )
Set(cmd_Midl_Files   )
Set(cmd_CustomBuild_Files   )

Set(ARCHIVE_OUTPUT_DIRECTORY ${ProjDir}/lib)

Add_Library(cmd STATIC  ${cmd_ClInclude_Files} ${cmd_ClCompile_Files} ${cmd_ResourceCompile_Files} ${cmd_Manifest_Files} ${cmd_Midl_Files} ${cmd_CustomBuild_Files})

set_target_properties(cmd PROPERTIES 
	COMPILE_DEFINITIONS "${TARGET_COMPILE_DEFS}"
	COMPILE_FLAGS "${TARGET_COMPILE_FLAGS}")

Set(cmd_Dependencies  )
target_link_libraries(cmd ${cmd_Dependencies})

