set(PROJECT qtalsavolume )
cmake_minimum_required( VERSION 2.8.7 )
if( UNIX AND NOT (APPLE OR CYGWIN) )
	add_definitions( -Wall )
	set( CMAKE_AUTOMOC ON )
	set( CMAKE_INCLUDE_CURRENT_DIR ON )
	set( USE_PULSE OFF CACHE BOOL "Set pulseaudio usage" )
	set( USE_QT5 OFF CACHE BOOL "Use Qt5" )
	set( BUILD_TRANSLATIONS ON CACHE BOOL "Build project translations")
	find_package( PkgConfig REQUIRED )
	pkg_check_modules( ALSA REQUIRED alsa )
	link_directories( ${ALSA_LIBRARY_DIRS} )
	if( "${DCMAKE_BUILD_TYPE}" STREQUAL "Debug" )
		add_definitions( -DISDEBUG )
	endif( "${DCMAKE_BUILD_TYPE}" STREQUAL "Debug" )
	set( DEPLIBS
		${ALSA_LIBRARIES}
	)
	include_directories(
		${CMAKE_SOURCE_DIR}
		${CMAKE_CURRENT_BINARY_DIR}
		${ALSA_INCLUDE_DIRS}
	)
	set( HEADERS
		alsawork/alsawork.h
		alsawork/mixerswitches.h
		alsawork/alsadevice.h
		defines.h
	)
	set( CPP_FILES
		main.cpp
		popupwindow.cpp
		alsawork/alsawork.cpp
		alsawork/mixerswitches.cpp
		alsawork/alsadevice.cpp
		settingsdialog.cpp
	)
	set( UI_FILES
		settingsdialog.ui
	)
	set( QRC_FILES
		icons.qrc
	)
	set( TRANSLATIONS_PATH languages )
	set( DATADIR_PATH share )
	set( APPDIR_PATH ${DATADIR_PATH}/applications )
	set( DESKTOP_FILES ${PROJECT}.desktop )
	set( ICON_FILES icons/volume_ico.png )
	set( DARK_ICONS
		icons/dark/tb_icon0.png
		icons/dark/tb_icon10.png
		icons/dark/tb_icon20.png
		icons/dark/tb_icon30.png
		icons/dark/tb_icon40.png
		icons/dark/tb_icon50.png
		icons/dark/tb_icon60.png
		icons/dark/tb_icon70.png
		icons/dark/tb_icon80.png
		icons/dark/tb_icon90.png
		icons/dark/tb_icon100.png
	)
	set( LIGHT_ICONS
		icons/light/tb_icon0.png
		icons/light/tb_icon10.png
		icons/light/tb_icon20.png
		icons/light/tb_icon30.png
		icons/light/tb_icon40.png
		icons/light/tb_icon50.png
		icons/light/tb_icon60.png
		icons/light/tb_icon70.png
		icons/light/tb_icon80.png
		icons/light/tb_icon90.png
		icons/light/tb_icon100.png
	)
	set( qtalsavolume_TRS
		${TRANSLATIONS_PATH}/${PROJECT}_en.ts
		${TRANSLATIONS_PATH}/${PROJECT}_ru.ts
		${TRANSLATIONS_PATH}/${PROJECT}_uk.ts
	)
	if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
		add_definitions( -DISDEBUG )
	endif( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
	if( ${USE_PULSE} )
		pkg_check_modules( PULSE REQUIRED libpulse )
		link_directories( ${PULSE_LIBRARY_DIRS} )
		include_directories( ${PULSE_INCLUDE_DIRS} )
		add_definitions( -DUSE_PULSE )
		set( HEADERS
			${HEADERS}
			pulsework/pulsecore.cpp
			pulsework/pulsedevice.cpp
		)
		set( CPP_FILES
			${CPP_FILES}
			pulsework/pulsecore.cpp
			pulsework/pulsedevice.cpp
		)
		set( DEPLIBS
			${DEPLIBS}
			${PULSE_LIBRARIES}
		)
	endif( ${USE_PULSE} )
	if( ${USE_QT5} )
		find_package ( Qt5Widgets REQUIRED )
		include_directories(
			${Qt5Widgets_INCLUDES}
		)
		set( CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}" )
		add_definitions( -DHAVE_QT5 )
		qt5_add_resources( RSCS ${QRC_FILES} )
		if( ${BUILD_TRANSLATIONS} )
			find_package( Qt5LinguistTools REQUIRED )
			qt5_add_translation( TRS ${qtalsavolume_TRS} )
		endif( ${BUILD_TRANSLATIONS} )
		qt5_wrap_ui( UIS ${UI_FILES} )
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
		QT4_WRAP_UI( UIS ${UI_FILES} )
		QT4_ADD_RESOURCES( RSCS ${QRC_FILES} )
		if( ${BUILD_TRANSLATIONS} )
			QT4_ADD_TRANSLATION( TRS ${qtalsavolume_TRS} )
		endif( ${BUILD_TRANSLATIONS} )
		set ( QT_DEPLIBS
			${QT_QTCORE_LIBRARY}
			${QT_QTGUI_LIBRARY}
		)
	endif( ${USE_QT5} )
	add_executable( ${PROJECT} ${CPP_FILES} ${UIS} ${RSCS} ${TRS} ${MOCS} "defines.h")
	target_link_libraries(
		${PROJECT}
		${QT_DEPLIBS}
		${DEPLIBS}
	)
	INSTALL( TARGETS ${PROJECT} RUNTIME DESTINATION bin )
	INSTALL( FILES ${DESKTOP_FILES} DESTINATION ${APPDIR_PATH} )
	INSTALL( FILES ${ICON_FILES} DESTINATION ${DATADIR_PATH}/${PROJECT}/icons )
	INSTALL( FILES ${DARK_ICONS} DESTINATION ${DATADIR_PATH}/${PROJECT}/icons/dark )
	INSTALL( FILES ${LIGHT_ICONS} DESTINATION ${DATADIR_PATH}/${PROJECT}/icons/light )
	INSTALL( FILES ${TRS} DESTINATION ${DATADIR_PATH}/${PROJECT}/${TRANSLATIONS_PATH} )
endif( UNIX AND NOT (APPLE OR CYGWIN) )
