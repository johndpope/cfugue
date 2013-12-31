###################################################
###  Auto-Generated with GPalem CMake Generator    
###                 from [Debug|Win32]
###  D:/WinApp/CarMusTy/src/References/CFugue/src/3rdparty/tse3/build/tse3ports.vcxproj
###################################################

cmake_minimum_required(VERSION 2.8)

Project(tse3ports)

Set(ProjDir ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(
	${ProjDir}/src/   )
link_directories(
	${ProjDir}/lib   )

Set(tse3ports_ClInclude_Files   )
Set(tse3ports_ClCompile_Files
	${ProjDir}/src/examples/ports/ports.cpp   )
Set(tse3ports_ResourceCompile_Files   )
Set(tse3ports_Manifest_Files   )
Set(tse3ports_Midl_Files   )
Set(tse3ports_CustomBuild_Files   )

Add_Executable(tse3ports ${tse3ports_ClInclude_Files} ${tse3ports_ClCompile_Files} ${tse3ports_ResourceCompile_Files} ${tse3ports_Manifest_Files} ${tse3ports_Midl_Files} ${tse3ports_CustomBuild_Files})

set_target_properties(tse3ports PROPERTIES 
	COMPILE_DEFINITIONS "${TARGET_COMPILE_DEFS}"
	COMPILE_FLAGS "${TARGET_COMPILE_FLAGS}")

Set(tse3ports_Dependencies  tse3)
target_link_libraries(tse3ports ${tse3ports_Dependencies})

