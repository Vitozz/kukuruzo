project( qtpoweroff )
cmake_minimum_required( VERSION 2.8.7 )
set( CMAKE_AUTOMOC ON )
set( CMAKE_INCLUDE_CURRENT_DIR ON )
set( USE_QT5 OFF CACHE BOOL "Use Qt5" )
set( BUILD_TRANSLATIONS ON CACHE BOOL "Build project translations")
include_directories(
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
)
set( qtpoweroff_SRC
	main.cpp
	mainwindow.cpp
)

set( qtpoweroff_HDR
	mainwindow.h
	defines.h
)
set( qtpoweroff_UI
	mainwindow.ui
)
set( qtpoweroff_QRC
	qtpoweroff.qrc
)
set( qtpoweroff_RC
	qtpoweroff.rc
)
set( LANG_PATH languages )
set( qtpoweroff_TRS
	${LANG_PATH}/qtpoweroff_ru.ts
	${LANG_PATH}/qtpoweroff_uk.ts
)
set( DOC_FILES
	README
)
set(LICENSE_FILES
	COPYING
)
if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
	add_definitions( -DIS_DEBUG )
endif( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
if( ${USE_QT5} )
	find_package( Qt5Widgets REQUIRED )
	include_directories(
		${Qt5Widgets_INCLUDES}
	)
	set( CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}" )
	add_definitions( -DHAVE_QT5 )
	qt5_add_resources( RSCS ${qtpoweroff_QRC} )
	if( ${BUILD_TRANSLATIONS} )
		find_package( Qt5LinguistTools REQUIRED )
		qt5_add_translation( TRS ${qtpoweroff_TRS} )
	endif( ${BUILD_TRANSLATIONS} )
	qt5_wrap_ui( UIS ${qtpoweroff_UI} )
	set( QT_DEPLIBS
		Qt5::Widgets
	)
	if( UNIX AND NOT (APPLE OR CYGWIN) )
		find_package( Qt5DBus REQUIRED )
		include_directories( ${Qt5DBus_INCLUDES} )
		add_definitions(
			${Qt5DBus_DEFINITIONS}
			-DHAVE_DBUS
		)
		set( QT_DEPLIBS
			${QT_DEPLIBS}
			Qt5::DBus
		)
	endif( UNIX AND NOT (APPLE OR CYGWIN) )
else( ${USE_QT5} )
	find_package( Qt4 REQUIRED )
	include( ${QT_USE_FILE} )
	include_directories(
		${QT_QTCORE_INCLUDE_DIR}
		${QT_QTGUI_INCLUDE_DIR}
	)
	QT4_WRAP_UI( UIS ${qtpoweroff_UI} )
	QT4_ADD_RESOURCES( RSCS ${qtpoweroff_QRC} )
	if( ${BUILD_TRANSLATIONS} )
		QT4_ADD_TRANSLATION( TRS ${qtpoweroff_TRS} )
	endif( ${BUILD_TRANSLATIONS} )
	set( QT_DEPLIBS
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
	)
	if( UNIX AND NOT (APPLE OR CYGWIN) )
		include_directories( ${QT_QTDBUS_INCLUDE_DIR} )
		add_definitions( -DHAVE_DBUS )
		set( QT_DEPLIBS
			${QT_DEPLIBS}
			${QT_QTDBUS_LIBRARY}
		)
	endif( UNIX AND NOT (APPLE OR CYGWIN) )
endif( ${USE_QT5} )
if( UNIX AND NOT (APPLE OR CYGWIN) )
	set( DATADIR_PATH ${CMAKE_INSTALL_PREFIX}/share )
	set( APPDATA_PATH ${DATADIR_PATH}/applications )
	set( ICON_FILES 
		images/tb_icon.png
	)
	set( ICON_PATH
		${DATADIR_PATH}/${PROJECT_NAME}/images
	)
	set( DESKTOP_FILES
		${PROJECT_NAME}.desktop
	)
	set ( DOCS_PATH
		${DATADIR_PATH}/doc/${PROJECT_NAME}
	)
	add_executable( ${PROJECT_NAME}
		${qtpoweroff_SRC}
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
	if( NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
		add_definitions( -DQT_NO_DEBUG )
	endif( NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
	set ( CMAKE_BUILD_WITH_INSTALL_RPATH FALSE )
	add_executable( ${PROJECT_NAME}
		WIN32
		${qtpoweroff_SRC}
		${UIS}
		${RSCS}
		${TRS}
		${qtpoweroff_RC}
	)
	target_link_libraries ( ${PROJECT_NAME}
				${QT_DEPLIBS}
	)
	INSTALL( TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX} )
	INSTALL( FILES ${TRS} DESTINATION ${CMAKE_INSTALL_PREFIX}/${LANG_PATH} )
	INSTALL( FILES ${DOC_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX} )
	INSTALL( FILES ${LICENSE_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX} )
endif( ${WIN32} )
