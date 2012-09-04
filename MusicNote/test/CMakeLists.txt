# CMakeLists.txt - Created with Sln2CMake Application


cmake_minimum_required(VERSION 2.6)

Message(STATUS "Buliding CFugue Tests")

if(NOT QT4_FOUND)
	FIND_PACKAGE(Qt4)
endif()

Set(ProjDir ${CMAKE_CURRENT_SOURCE_DIR})

#################################
# Compute Values based on Options
#################################
set(Char_Flags "" CACHE INTERNAL "Compiler Character Flags: Unicode/MBCS Options")
set(Char_postfix "" CACHE INTERNAL "Unicode Postfix")
set(Output_postfix "" CACHE INTERNAL "Debug Postfix")

if(CFUGUE_UNICODE_BUILD)
	SET(Char_postfix "u")
	SET(Char_Flags "UNICODE;_UNICODE")
	message(STATUS "CFugue Tests: Unicode build is ON")
else(CFUGUE_UNICODE_BUILD)
	SET(Char_postfix "")
	SET(Char_Flags "_MBCS;")
	message(STATUS "CFugue Tests: Unicode build is OFF")
endif(CFUGUE_UNICODE_BUILD)

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "Debug" CACHE STRING
        "Choose the type of build, options are: Debug Release"
        FORCE)   
ENDIF(NOT CMAKE_BUILD_TYPE)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
	SET(Output_postfix "")	
endif()
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	SET(Output_postfix "d")	
endif()
		
SET(CMAKE_DEBUG_POSTFIX "d${Char_postfix}")
SET(CMAKE_RELEASE_POSTFIX "${Char_postfix}")

SET(TARGET_COMPILE_DEFS "${Char_Flags};_CRT_SECURE_NO_DEPRECATE;_CRT_SECURE_NO_WARNINGS;_BIND_TO_CURRENT_CRT_VERSION")

IF(CMAKE_COMPILER_IS_GNUCC)
  SET(TARGET_COMPILE_FLAGS "-std=gnu++11") # Add C++0x support for GCC
ENDIF()

IF(MSVC)
  SET(TARGET_COMPILE_FLAGS "/Zc:wchar_t- /Zc:forScope")
ENDIF(MSVC)

#################################
# Setup Output Directories
#################################
SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG
  ${PROJECT_SOURCE_DIR}/bin
  CACHE PATH
  "Destination Directory for output Shared Libraries (Debug Mode)"
  )
SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE
  ${PROJECT_SOURCE_DIR}/bin
  CACHE PATH
  "Destination Directory for output Shared Libraries (Release Mode)"
  )

SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG
  ${PROJECT_SOURCE_DIR}/bin
  CACHE PATH
  "Destination Directory for output Executables (Debug Mode)."
  )
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE
  ${PROJECT_SOURCE_DIR}/bin
  CACHE PATH
  "Destination Directory for output Executables (Release Mode)."
  )

SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG
  ${PROJECT_SOURCE_DIR}/lib
  CACHE PATH
  "Destination Directory for output static libraries (Debug Mode)."
  )
SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE
  ${PROJECT_SOURCE_DIR}/lib
  CACHE PATH
  "Destination Directory for output static libraries (Release Mode)."
  )

mark_as_advanced(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG)
mark_as_advanced(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE)
mark_as_advanced(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG)
mark_as_advanced(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE)
mark_as_advanced(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG)
mark_as_advanced(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE)


include_directories("${ProjDir}/../include" 
					"${ProjDir}/../include/Common" 
					"${ProjDir}/../src" 
					"${ProjDir}/../src/QtVuMeter"
					 ${TSE3_INCLUDE_DIR}
					"${CMAKE_CURRENT_BINARY_DIR}"
					"${ProjDir}/../src/3rdparty/tse3/src"	# for tse3play
					)
if(QT4_FOUND)
INCLUDE(${QT_USE_FILE})
endif()

#################################
#### Target: testCFugueDll   ####
#################################
SET( DLLTestApp_Source_Files 
	${ProjDir}/DLLTestApp/DLLTestApp.cpp
	${ProjDir}/DLLTestApp/stdafx.cpp
   )
SET( DLLTestApp_Header_Files 
	${ProjDir}/DLLTestApp/stdafx.h
	${ProjDir}/DLLTestApp/targetver.h
   )
SET( DLLTestApp_UnKnownGroup ${ProjDir}/DLLTestApp/ReadMe.txt    )

if (BUILD_CFUGUE_TESTS AND BUILD_CFUGUE_DLL)
	add_executable(testCFugueDll   ${DLLTestApp_Source_Files}  ${DLLTestApp_Header_Files}  ${DLLTestApp_UnKnownGroup} )
	SET_TARGET_PROPERTIES(testCFugueDll PROPERTIES COMPILE_DEFINITIONS "${TARGET_COMPILE_DEFS}" COMPILE_FLAGS "${TARGET_COMPILE_FLAGS}")
	
	SET(DLLTestApp_Dependencies MusicNoteDLL  ${DLLTestApp_Librarian} )
	target_link_libraries(testCFugueDll  ${DLLTestApp_Dependencies})
	install(TARGETS testCFugueDll RUNTIME DESTINATION bin  LIBRARY DESTINATION bin ARCHIVE DESTINATION lib)
endif (BUILD_CFUGUE_TESTS AND BUILD_CFUGUE_DLL)


#################################
#### Target: testCFugueLib   ####
#################################
SET( StaticLibTestApp_Source_Files 
	${ProjDir}/StaticLibTestApp/SampleApp.cpp
	${ProjDir}/StaticLibTestApp/stdafx.cpp
   )
SET( StaticLibTestApp_Header_Files 
	${ProjDir}/StaticLibTestApp/stdafx.h
	${ProjDir}/StaticLibTestApp/targetver.h
   )

	add_executable(testCFugueLib   ${StaticLibTestApp_Source_Files}  ${StaticLibTestApp_Header_Files} )
	SET_TARGET_PROPERTIES(testCFugueLib PROPERTIES COMPILE_DEFINITIONS "${TARGET_COMPILE_DEFS}" COMPILE_FLAGS "${TARGET_COMPILE_FLAGS}")
	SET(StaticLibTestApp_Dependencies CFugue  ${StaticLibTestApp_Librarian} )
	target_link_libraries(testCFugueLib  ${StaticLibTestApp_Dependencies})
	install(TARGETS testCFugueLib RUNTIME DESTINATION bin  LIBRARY DESTINATION bin ARCHIVE DESTINATION lib)
	
#################################
#### Target: QtVuMeter       ####
#################################		
if(QT4_FOUND)
	SET(QtVuMeter_HEADERS	${ProjDir}/../src/QtVuMeter/qvumeter.h								
							${ProjDir}/../src/QtVuMeter/midioutcombobox.h
							${ProjDir}/../src/QtVuMeter/midiincombobox.h)
	SET(QtVuMeter_FORMS		)
	SET(QtVuMeter_SOURCES 	${ProjDir}/../src/QtVuMeter/qvumeter.cpp
							${ProjDir}/../src/QtVuMeter/midioutcombobox.cpp
							${ProjDir}/../src/QtVuMeter/midiincombobox.cpp)

	QT4_WRAP_CPP(QtVuMeter_HEADERS_MOC ${QtVuMeter_HEADERS})
	QT4_WRAP_UI(QtVuMeter_FORMS_HEADERS ${QtVuMeter_FORMS})
	QT4_ADD_RESOURCES(QtVuMeter_RESOURCES_RCC ${QtVuMeter_RESOURCES})

	SOURCE_GROUP(Qt_Forms		FILES ${QtVuMeter_FORMS})
	SOURCE_GROUP(Resources		FILES ${QtVuMeter_RESOURCES})
	SOURCE_GROUP(Header_Files	FILES ${QtVuMeter_HEADERS})
	SOURCE_GROUP(Translations	FILES ${QtVuMeter_TRANSLATIONS})
	SOURCE_GROUP(Generated_Files FILES ${QtVuMeter_HEADERS_MOC} ${QtVuMeter_FORMS_HEADERS} ${QtVuMeter_RESOURCES_RCC})

	add_library(QtVuMeter ${QtVuMeter_HEADERS} ${QtVuMeter_SOURCES} ${QtVuMeter_HEADERS_MOC} ${QtVuMeter_FORMS_HEADERS} ${QtVuMeter_RESOURCES_RCC})
	set_target_properties(QtVuMeter PROPERTIES COMPILE_DEFINITIONS "${TARGET_COMPILE_DEFS}" COMPILE_FLAGS "${TARGET_COMPILE_FLAGS}")

	SET( QtVuMeter_Dependencies  ${QT_LIBRARIES})
	target_link_libraries(QtVuMeter  ${QtVuMeter_Dependencies})
	install(TARGETS QtVuMeter RUNTIME DESTINATION bin  LIBRARY DESTINATION bin ARCHIVE DESTINATION lib)
endif(QT4_FOUND)
	
#################################
#### Target: testQtVuMeter   ####
#################################	
if(QT4_FOUND)
	SET(External_HEADERS	${ProjDir}/../src/QtVuMeter/qvumeter.h)
	SET(SampleVU_HEADERS	SampleVU/SampleVU_mainwindow.h)
	SET(SampleVU_FORMS		SampleVU/SampleVU_mainwindow.ui)
	SET(SampleVU_SOURCES 	SampleVU/SampleVU_mainwindow.cpp 
							SampleVU/main.cpp)
	SET(SampleVU_RESOURCES	 )
	SET(SampleVU_TRANSLATIONS )

	QT4_WRAP_CPP(SampleVU_HEADERS_MOC ${SampleVU_HEADERS})
	QT4_WRAP_UI(SampleVU_FORMS_HEADERS ${SampleVU_FORMS})
	QT4_ADD_RESOURCES(SampleVU_RESOURCES_RCC ${SampleVU_RESOURCES})

	SOURCE_GROUP(Qt_Forms		FILES ${SampleVU_FORMS})
	SOURCE_GROUP(Resources		FILES ${SampleVU_RESOURCES})
	SOURCE_GROUP(Header_Files	FILES ${SampleVU_HEADERS} ${External_HEADERS})
	SOURCE_GROUP(Translations	FILES ${SampleVU_TRANSLATIONS})
	SOURCE_GROUP(Generated_Files FILES ${SampleVU_HEADERS_MOC} ${SampleVU_FORMS_HEADERS} ${SampleVU_RESOURCES_RCC})

	ADD_EXECUTABLE(testQtVuMeter WIN32 ${External_HEADERS} ${SampleVU_HEADERS} ${SampleVU_SOURCES} ${SampleVU_HEADERS_MOC} ${SampleVU_FORMS_HEADERS} ${SampleVU_RESOURCES_RCC})
	set_target_properties(testQtVuMeter PROPERTIES COMPILE_DEFINITIONS "${TARGET_COMPILE_DEFS}" COMPILE_FLAGS "${TARGET_COMPILE_FLAGS}")

	SET( SampleVU_Dependencies QtVuMeter ${QT_LIBRARIES} ${QT_QTMAIN_LIBRARY})
	target_link_libraries(testQtVuMeter  ${SampleVU_Dependencies})
	install(TARGETS testQtVuMeter RUNTIME DESTINATION bin  LIBRARY DESTINATION bin ARCHIVE DESTINATION lib)
endif(QT4_FOUND)


#################################
#### Target: QtTse3PlayerApp ####
#################################
if(QT4_FOUND)
	SET(External_HEADERS	${ProjDir}/../src/QtVuMeter/qvumeter.h								
							${ProjDir}/../src/QtVuMeter/midioutcombobox.h
							${ProjDir}/../src/QtVuMeter/midiincombobox.h
							QtTestApp/QtTse3Play.h)
	SET(QtTestApp_HEADERS	QtTestApp/qttse3playvisual.h)
	SET(QtTestApp_FORMS		QtTestApp/qttse3playvisual.ui)
	SET(QtTestApp_SOURCES 	QtTestApp/QtTse3Play.cpp
							QtTestApp/qttse3playvisual.cpp
							QtTestApp/main.cpp)
	SET(QtTestApp_RESOURCES	 )
	SET(QtTestApp_TRANSLATIONS )

	QT4_WRAP_CPP(QtTestApp_HEADERS_MOC ${QtTestApp_HEADERS})
	QT4_WRAP_UI(QtTestApp_FORMS_HEADERS ${QtTestApp_FORMS})
	QT4_ADD_RESOURCES(QtTestApp_RESOURCES_RCC ${QtTestApp_RESOURCES})

	SOURCE_GROUP(Qt_Forms		FILES ${QtTestApp_FORMS})
	SOURCE_GROUP(Resources		FILES ${QtTestApp_RESOURCES})
	SOURCE_GROUP(Header_Files	FILES ${QtTestApp_HEADERS} ${External_HEADERS})
	SOURCE_GROUP(Translations	FILES ${QtTestApp_TRANSLATIONS})
	SOURCE_GROUP(Generated_Files FILES ${QtTestApp_HEADERS_MOC} ${QtTestApp_FORMS_HEADERS} ${QtTestApp_RESOURCES_RCC})

	ADD_EXECUTABLE(tse3QtPlayer WIN32 ${External_HEADERS} ${QtTestApp_HEADERS} ${QtTestApp_SOURCES} ${QtTestApp_HEADERS_MOC} ${QtTestApp_FORMS_HEADERS} ${QtTestApp_RESOURCES_RCC})
	set_target_properties(tse3QtPlayer PROPERTIES COMPILE_DEFINITIONS "${TARGET_COMPILE_DEFS}" COMPILE_FLAGS "${TARGET_COMPILE_FLAGS}")

	SET( QtTestApp_Dependencies QtVuMeter CFugue ${TSE3_LIBRARIES} ${QT_LIBRARIES} ${QT_QTMAIN_LIBRARY})
	target_link_libraries(tse3QtPlayer  ${QtTestApp_Dependencies})
	install(TARGETS tse3QtPlayer RUNTIME DESTINATION bin  LIBRARY DESTINATION bin ARCHIVE DESTINATION lib)
endif(QT4_FOUND)

#################################
#### Target: Documentation ####
#################################

#################################
#Link the targets with their Depenedency Libraries
#################################

