# CMakeLists.txt - Created with Sln2CMake Application


cmake_minimum_required(VERSION 2.6)

Message(STATUS "Buliding CFugue Tests")

Set(ProjDir ${CMAKE_CURRENT_SOURCE_DIR})

# Set path to additional CMake modules
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${ProjDir}/../cmake/modules/")

FIND_PACKAGE(Qt4)
FIND_PACKAGE(Tse3)

include_directories( "${ProjDir}/../include" "${ProjDir}/../include/Common" )

SET(TARGET_COMPILE_DEFS "${Char_Flags};_CRT_SECURE_NO_DEPRECATE;_CRT_SECURE_NO_WARNINGS;_BIND_TO_CURRENT_CRT_VERSION")
IF(MSVC)
  SET(TARGET_COMPILE_FLAGS "/Zc:wchar_t- /Zc:forScope")
ENDIF(MSVC)

#################################
#### Target: DLLTestApp ####
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
	add_executable(DLLTestApp   ${DLLTestApp_Source_Files}  ${DLLTestApp_Header_Files}  ${DLLTestApp_UnKnownGroup} )
	SET_TARGET_PROPERTIES(DLLTestApp PROPERTIES DEBUG_POSTFIX "d${Char_postfix}" RELEASE_POSTFIX "${Char_postfix}")
	
	SET(DLLTestApp_Dependencies MusicNoteDLL  ${DLLTestApp_Librarian} )
	target_link_libraries(DLLTestApp  ${DLLTestApp_Dependencies})
	install(TARGETS DLLTestApp RUNTIME DESTINATION bin  LIBRARY DESTINATION bin ARCHIVE DESTINATION lib)
endif (BUILD_CFUGUE_TESTS AND BUILD_CFUGUE_DLL)


#################################
#### Target: StaticTestApp ####
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
	SET_TARGET_PROPERTIES(testCFugueLib PROPERTIES DEBUG_POSTFIX "d${Char_postfix}" RELEASE_POSTFIX "${Char_postfix}")
	SET(StaticLibTestApp_Dependencies libCFugue  ${StaticLibTestApp_Librarian} )
	target_link_libraries(testCFugueLib  ${StaticLibTestApp_Dependencies})
	install(TARGETS testCFugueLib RUNTIME DESTINATION bin  LIBRARY DESTINATION bin ARCHIVE DESTINATION lib)
	
#################################
#### Target: SampleVU        ####
#################################	
if(QT4_FOUND)
INCLUDE(${QT_USE_FILE})
	include_directories( ${ProjDir}/../"include" ${ProjDir}/../"src" ${CMAKE_CURRENT_BINARY_DIR} ${ProjDir}/../src/QtVuMeter)
	SET(SampleVU_HEADERS	SampleVU/SampleVU_mainwindow.h ${ProjDir}/../src/QtVuMeter/qvumeter.h)
	SET(SampleVU_FORMS		SampleVU/SampleVU_mainwindow.ui)
	SET(SampleVU_SOURCES 	SampleVU/SampleVU_mainwindow.cpp 
							SampleVU/main.cpp
							${ProjDir}/../src/QtVuMeter/qvumeter.cpp)
	SET(SampleVU_RESOURCES	 )
	SET(SampleVU_TRANSLATIONS )

	QT4_WRAP_CPP(SampleVU_HEADERS_MOC ${SampleVU_HEADERS})
	QT4_WRAP_UI(SampleVU_FORMS_HEADERS ${SampleVU_FORMS})
	QT4_ADD_RESOURCES(SampleVU_RESOURCES_RCC ${SampleVU_RESOURCES})

	SOURCE_GROUP(Qt_Forms		FILES ${SampleVU_FORMS})
	SOURCE_GROUP(Resources		FILES ${SampleVU_RESOURCES})
	SOURCE_GROUP(Header_Files	FILES ${SampleVU_HEADERS})
	SOURCE_GROUP(Translations	FILES ${SampleVU_TRANSLATIONS})
	SOURCE_GROUP(Generated_Files FILES ${SampleVU_HEADERS_MOC} ${SampleVU_FORMS_HEADERS} ${SampleVU_RESOURCES_RCC})

	ADD_EXECUTABLE(SampleVU WIN32 ${SampleVU_HEADERS} ${SampleVU_SOURCES} ${SampleVU_HEADERS_MOC} ${SampleVU_FORMS_HEADERS} ${SampleVU_RESOURCES_RCC})
	set_target_properties(SampleVU PROPERTIES  DEBUG_POSTFIX "d${Char_postfix}" RELEASE_POSTFIX "${Char_postfix}"
									COMPILE_DEFINITIONS "${TARGET_COMPILE_DEFS}"
									COMPILE_FLAGS "${TARGET_COMPILE_FLAGS}")

	SET( SampleVU_Dependencies libCFugue ${QT_LIBRARIES} ${QT_QTMAIN_LIBRARY})
	target_link_libraries(SampleVU  ${SampleVU_Dependencies})
	install(TARGETS SampleVU RUNTIME DESTINATION bin  LIBRARY DESTINATION bin ARCHIVE DESTINATION lib)
endif(QT4_FOUND)


#################################
#### Target: QtTse3PlayerApp ####
#################################
if(QT4_FOUND)
INCLUDE(${QT_USE_FILE})
	include_directories( ${ProjDir}/../"include" ${ProjDir}/../"src" ${CMAKE_CURRENT_BINARY_DIR} ${ProjDir}/../src/QtVuMeter ${ProjDir}/../src/3rdparty/tse3/src)
	SET(QtTestApp_HEADERS	QtTestApp/qttse3playvisual.h 
							QtTestApp/QtTse3Play.h 
							${ProjDir}/../src/QtVuMeter/qvumeter.h								
							${ProjDir}/../src/QtVuMeter/midioutcombobox.h
							${ProjDir}/../src/QtVuMeter/midiincombobox.h)
	SET(QtTestApp_FORMS		QtTestApp/qttse3playvisual.ui)
	SET(QtTestApp_SOURCES 	QtTestApp/QtTse3Play.cpp
							QtTestApp/qttse3playvisual.cpp
							QtTestApp/main.cpp
							${ProjDir}/../src/QtVuMeter/qvumeter.cpp
							${ProjDir}/../src/QtVuMeter/midioutcombobox.cpp
							${ProjDir}/../src/QtVuMeter/midiincombobox.cpp)
	SET(QtTestApp_RESOURCES	 )
	SET(QtTestApp_TRANSLATIONS )

	QT4_WRAP_CPP(QtTestApp_HEADERS_MOC ${QtTestApp_HEADERS})
	QT4_WRAP_UI(QtTestApp_FORMS_HEADERS ${QtTestApp_FORMS})
	QT4_ADD_RESOURCES(QtTestApp_RESOURCES_RCC ${QtTestApp_RESOURCES})

	SOURCE_GROUP(Qt_Forms		FILES ${QtTestApp_FORMS})
	SOURCE_GROUP(Resources		FILES ${QtTestApp_RESOURCES})
	SOURCE_GROUP(Header_Files	FILES ${QtTestApp_HEADERS})
	SOURCE_GROUP(Translations	FILES ${QtTestApp_TRANSLATIONS})
	SOURCE_GROUP(Generated_Files FILES ${QtTestApp_HEADERS_MOC} ${QtTestApp_FORMS_HEADERS} ${QtTestApp_RESOURCES_RCC})

	ADD_EXECUTABLE(tse3QtPlayer WIN32 ${QtTestApp_HEADERS} ${QtTestApp_SOURCES} ${QtTestApp_HEADERS_MOC} ${QtTestApp_FORMS_HEADERS} ${QtTestApp_RESOURCES_RCC})
	set_target_properties(tse3QtPlayer PROPERTIES  DEBUG_POSTFIX "d${Char_postfix}" RELEASE_POSTFIX "${Char_postfix}"
									COMPILE_DEFINITIONS "${TARGET_COMPILE_DEFS}"
									COMPILE_FLAGS "${TARGET_COMPILE_FLAGS}")

	SET( QtTestApp_Dependencies libCFugue tse3 ${QT_LIBRARIES} ${QT_QTMAIN_LIBRARY})
	target_link_libraries(tse3QtPlayer  ${QtTestApp_Dependencies})
	install(TARGETS tse3QtPlayer RUNTIME DESTINATION bin  LIBRARY DESTINATION bin ARCHIVE DESTINATION lib)
endif(QT4_FOUND)

#################################
#### Target: Documentation ####
#################################

#################################
#Link the targets with their Depenedency Libraries
#################################

