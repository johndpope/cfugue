# CMakeLists.txt


cmake_minimum_required(VERSION 2.6)

PROJECT( CFugueDLL )

SET(TARGET_COMPILE_DEFS "${Char_Flags};_USRDLL;MUSICNOTEDLL_EXPORTS;_ATL_DLL;_CRT_SECURE_NO_DEPRECATE;_CRT_SECURE_NO_WARNINGS;PHTRANSLATE_STATICLIB;_BIND_TO_CURRENT_CRT_VERSION")
IF(MSVC)
  SET(TARGET_COMPILE_FLAGS "/Zl")
ENDIF(MSVC)

#################################
#### Target: CFugueDLL ####
#################################
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../../include)

SET( CFugueDLL_Source_Files 
	dlldatax.c
	dllmain.cpp
	CFugueDLL.cpp
	CFugueDLL.def
	CFugueDLL.idl
	../CFugueLib/CFugueLib.cpp
	stdafx.cpp
   )  
SET( CFugueDLL_Header_Files 
	dlldatax.h
	dllmain.h
	../../include/CFugueDll.h
	Resource.h
	stdafx.h
	targetver.h
   )
SET( CFugueDLL_Resource_Files 
	#CFugueDLL.rc
	CFugueDLL.rgs
   )
# SET( CFugueDLL_Generated_Files 
	# #${CMAKE_CURRENT_BINARY_DIR}/CFugueDLL_p.c
	# ${CMAKE_CURRENT_BINARY_DIR}/CFugueDLL_i.c
	# ${CMAKE_CURRENT_BINARY_DIR}/CFugueDLL_p.h
   # )
SET( CFugueDLL_UnKnownGroup ReadMe.txt    )

set(BUILD_CFUGUE_DLL TRUE)

if(BUILD_CFUGUE_DLL)
	add_library(CFugueDLL  SHARED   ${CFugueDLL_Source_Files}  ${CFugueDLL_Header_Files}  ${CFugueDLL_Resource_Files}  ${CFugueDLL_Generated_Files}  ${CFugueDLL_UnKnownGroup} )
	set_target_properties(CFugueDLL PROPERTIES 
									COMPILE_DEFINITIONS "${TARGET_COMPILE_DEFS}"
									COMPILE_FLAGS "${TARGET_COMPILE_FLAGS}")	
	if (WIN32)
		SET( CFugueDLL_Dependencies CFugueLib  ${CFugueDLL_Librarian} )
		target_link_libraries(CFugueDLL  ${CFugueDLL_Dependencies})
		install(TARGETS CFugueDLL RUNTIME DESTINATION bin  LIBRARY DESTINATION bin ARCHIVE DESTINATION lib)
	ELSE(WIN32)
 		MESSAGE("CFugueDLL is a COM component - only supported on Windows builds !!")
	ENDIF(WIN32)
ENDIF(BUILD_CFUGUE_DLL)



