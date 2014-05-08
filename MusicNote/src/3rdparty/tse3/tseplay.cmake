###################################################
###  Auto-Generated with GPalem CMake Generator    
###                 from [Debug|Win32]
###  D:/WinApp/CarMusTy/src/References/CFugue/src/3rdparty/tse3/build/tseplay.vcxproj
###################################################

cmake_minimum_required(VERSION 2.8)

Project(tseplay)

Set(ProjDir ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(
	${ProjDir}/src   )
link_directories(
	${ProjDir}/lib/   )

Set(tseplay_ClInclude_Files
	${ProjDir}/src/tse3play/tse3play.h   )
Set(tseplay_ClCompile_Files
	${ProjDir}/src/tse3play/main.cpp
	${ProjDir}/src/tse3play/tse3play.cpp   )
Set(tseplay_ResourceCompile_Files   )
Set(tseplay_Manifest_Files   )
Set(tseplay_Midl_Files   )
Set(tseplay_CustomBuild_Files   )

Set(RUNTIME_OUTPUT_DIRECTORY ${ProjDir}/bin)

Add_Executable(tse3play ${tseplay_ClInclude_Files} ${tseplay_ClCompile_Files} ${tseplay_ResourceCompile_Files} ${tseplay_Manifest_Files} ${tseplay_Midl_Files} ${tseplay_CustomBuild_Files})

set_target_properties(tse3play PROPERTIES 
	COMPILE_DEFINITIONS "${TARGET_COMPILE_DEFS}"
	COMPILE_FLAGS "${TARGET_COMPILE_FLAGS}")

if(WIN32)
	Set(tseplay_Dependencies  WinMM.lib)
endif()

Set(tseplay_Dependencies  ${tseplay_Dependencies} tse3)
target_link_libraries(tse3play ${tseplay_Dependencies})

