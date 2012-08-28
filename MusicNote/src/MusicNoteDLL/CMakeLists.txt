# CMakeLists.txt


cmake_minimum_required(VERSION 2.6)

#PROJECT( MusicNoteDLL )

SET(TARGET_COMPILE_DEFS "${Char_Flags};_USRDLL;MUSICNOTEDLL_EXPORTS;_ATL_DLL;_CRT_SECURE_NO_DEPRECATE;_CRT_SECURE_NO_WARNINGS;PHTRANSLATE_STATICLIB;_BIND_TO_CURRENT_CRT_VERSION")
IF(MSVC)
  SET(TARGET_COMPILE_FLAGS "/Zl")
ENDIF(MSVC)

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
#MESSAGE(TODO:Add IDL Type processing to the CMake for CFugueDLL)
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

if(BUILD_CFUGUE_DLL)
	add_library(CFugueDLL  SHARED   ${MusicNoteDLL_Source_Files}  ${MusicNoteDLL_Header_Files}  ${MusicNoteDLL_Resource_Files}  ${MusicNoteDLL_Generated_Files}  ${MusicNoteDLL_UnKnownGroup} )
	set_target_properties(CFugueDLL PROPERTIES 
									COMPILE_DEFINITIONS "${TARGET_COMPILE_DEFS}"
									COMPILE_FLAGS "${TARGET_COMPILE_FLAGS}")	
	if (WIN32)
		SET( MusicNoteDLL_Dependencies CFugueLib  ${MusicNoteDLL_Librarian} )
		target_link_libraries(CFugueDLL  ${MusicNoteDLL_Dependencies})
		install(TARGETS CFugueDLL RUNTIME DESTINATION bin  LIBRARY DESTINATION bin ARCHIVE DESTINATION lib)
	ELSE(WIN32)
 		MESSAGE("CFugueDLL is a COM component - only supported on Windows builds !!")
	ENDIF(WIN32)
ENDIF(BUILD_CFUGUE_DLL)



