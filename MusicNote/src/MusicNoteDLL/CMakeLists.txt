# CMakeLists.txt - Created with Sln2CMake Application


cmake_minimum_required(VERSION 2.6)

# PROJECT( MusicNoteDLL )

       
ADD_DEFINITIONS(-D_USRDLL -DMUSICNOTEDLL_EXPORTS)

#################################
#### Target: MusicNoteDLL ####
#################################
SET( MusicNoteDLL_Source_Files 
	dlldatax.c
	dllmain.cpp
	MusicNoteDLL.cpp
	MusicNoteDLL.def
	MusicNoteDLL.idl
	../MusicNoteLib/MusicNoteLib.cpp
	stdafx.cpp
   )
MESSAGE(TODO:Add IDL Type processing to the CMake for MusicNoteDll)
SET( MusicNoteDLL_Header_Files 
	dlldatax.h
	dllmain.h
	../../include/MusicNoteDll.h
	Resource.h
	stdafx.h
	targetver.h
   )
SET( MusicNoteDLL_Resource_Files 
	#MusicNoteDLL.rc
	MusicNoteDLL.rgs
   )
SET( MusicNoteDLL_Generated_Files 
	MusicNoteDLL_i.c
	MusicNoteDLL_i.h
   )
SET( MusicNoteDLL_UnKnownGroup ReadMe.txt    )

if(BUILD_MUSICNOTE_DLL)
	add_library(MusicNoteDLL  SHARED   ${MusicNoteDLL_Source_Files}  ${MusicNoteDLL_Header_Files}  ${MusicNoteDLL_Resource_Files}  ${MusicNoteDLL_Generated_Files}  ${MusicNoteDLL_UnKnownGroup} )
	SET_TARGET_PROPERTIES(MusicNoteDLL PROPERTIES DEBUG_POSTFIX "d")
ENDIF(BUILD_MUSICNOTE_DLL)


#################################
#Link the targets with their Depenedency Libraries
#################################


if (BUILD_MUSICNOTE_DLL)
	SET( MusicNoteDLL_Dependencies MusicNoteLib  ${MusicNoteDLL_Librarian} )
	target_link_libraries(MusicNoteDLL  ${MusicNoteDLL_Dependencies})
ENDIF (BUILD_MUSICNOTE_DLL)


