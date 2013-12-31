###################################################
###  Auto-Generated with GPalem CMake Generator    
###                 from [Debug|Win32]
###  D:/WinApp/CarMusTy/src/References/CFugue/src/3rdparty/tse3/build/ins.vcxproj
###################################################

cmake_minimum_required(VERSION 2.8)

Project(ins)

Set(ProjDir ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(
	${ProjDir}/src/   )
link_directories(   )

Set(ins_ClInclude_Files
	${ProjDir}/src/tse3/ins/Destination.h
	${ProjDir}/src/tse3/ins/Instrument.h   )
Set(ins_ClCompile_Files
	${ProjDir}/src/tse3/ins/Destination.cpp
	${ProjDir}/src/tse3/ins/Instrument.cpp   )
Set(ins_ResourceCompile_Files   )
Set(ins_Manifest_Files   )
Set(ins_Midl_Files   )
Set(ins_CustomBuild_Files   )

Add_Library(ins STATIC  ${ins_ClInclude_Files} ${ins_ClCompile_Files} ${ins_ResourceCompile_Files} ${ins_Manifest_Files} ${ins_Midl_Files} ${ins_CustomBuild_Files})

set_target_properties(ins PROPERTIES 
	COMPILE_DEFINITIONS "${TARGET_COMPILE_DEFS}"
	COMPILE_FLAGS "${TARGET_COMPILE_FLAGS}")

Set(ins_Dependencies  )
target_link_libraries(ins ${ins_Dependencies})

