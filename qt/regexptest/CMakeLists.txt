project( regexptest )
cmake_minimum_required( VERSION 2.8.11 )
set( CMAKE_AUTOMOC ON )
set( CMAKE_INCLUDE_CURRENT_DIR ON )
set( USE_QT5 OFF CACHE BOOL "Use Qt5" )
include_directories(
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
)
set( regexptest_SRC
	main.cpp
	mainwindow.cpp
)

set( regexptest_HDR
	mainwindow.h
)
set( regexptest_UI
	mainwindow.ui
)
set( regexptest_QRC
	resources.qrc
)
set( regexptest_RC
	MyApp.rc
)
set( LANG_PATH langs )
set( regexptest_TRS
	${LANG_PATH}/${PROJECT_NAME}_ru.ts
	${LANG_PATH}/${PROJECT_NAME}_uk.ts
)
set( DOC_FILES
	docs/regexp_help.html
)
set(LICENSE_FILES
	COPYING
)
if( ${USE_QT5} )
	find_package( Qt5Widgets REQUIRED )
	find_package( Qt5LinguistTools REQUIRED )
	include_directories(
		${Qt5Widgets_INCLUDES}
	)
	set( CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}" )
	add_definitions( -DHAVE_QT5 )
	qt5_add_resources( RSCS ${regexptest_QRC} )
	qt5_add_translation( TRS ${regexptest_TRS} )
	qt5_wrap_ui( UIS ${regexptest_UI} )
	set( QT_DEPLIBS
		Qt5::Widgets
	)
else( ${USE_QT5} )
	find_package( Qt4 REQUIRED )
	include( ${QT_USE_FILE} )
	include_directories(
		${QT_QTCORE_INCLUDE_DIR}
		${QT_QTGUI_INCLUDE_DIR}
	)
	QT4_WRAP_UI( UIS ${regexptest_UI} )
	QT4_ADD_RESOURCES( RSCS ${regexptest_QRC} )
	QT4_ADD_TRANSLATION( TRS ${regexptest_TRS} )
	set( QT_DEPLIBS
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
	)
endif( ${USE_QT5} )
if( UNIX AND NOT (APPLE OR CYGWIN) )
	set( DATADIR_PATH ${CMAKE_INSTALL_PREFIX}/share )
	set( APPDATA_PATH ${DATADIR_PATH}/applications )
	set( ICON_FILES 
		icons/72x72.png
	)
	set( ICON_PATH
		${DATADIR_PATH}/${PROJECT_NAME}/icons
	)
	set( DESKTOP_FILES
		${PROJECT_NAME}.desktop
	)
	set ( DOCS_PATH
		${DATADIR_PATH}/doc/${PROJECT_NAME}/html
	)
	add_executable( ${PROJECT_NAME}
		${regexptest_SRC}
		${UIS}
		${RSCS}
		${TRS}
	)
	target_link_libraries ( ${PROJECT_NAME}
				${QT_DEPLIBS}
	)
	INSTALL( TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
	INSTALL( FILES ${ICON_FILES} DESTINATION ${ICON_PATH} )
	INSTALL( FILES ${DESKTOP_FILES} DESTINATION ${APPDATA_PATH} )
	INSTALL( FILES ${DOC_FILES} DESTINATION ${DOCS_PATH} )
	INSTALL( FILES ${TRS} DESTINATION ${DATADIR_PATH}/${PROJECT_NAME}/${LANG_PATH} )
endif( UNIX AND NOT (APPLE OR CYGWIN) )
if( ${WIN32} )
	set ( CMAKE_BUILD_WITH_INSTALL_RPATH FALSE )
	add_executable( ${PROJECT_NAME}
		WIN32
		${regexptest_SRC}
		${UIS}
		${RSCS}
		${TRS}
		regexptest_RC
	)
	target_link_libraries ( ${PROJECT_NAME}
				${QT_DEPLIBS}
	)
	INSTALL( TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX} )
	INSTALL( FILES ${TRS} DESTINATION ${CMAKE_INSTALL_PREFIX}/langs )
	INSTALL( FILES ${DOC_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/docs )
	INSTALL( FILES ${LICENSE_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX} )
endif( ${WIN32} )
