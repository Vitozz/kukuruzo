project(qtalsavolume )
cmake_minimum_required( VERSION 2.8.12 )
if( UNIX AND NOT (APPLE OR CYGWIN) )
	set( CMAKE_AUTOMOC ON )
	set( CMAKE_INCLUDE_CURRENT_DIR ON )
	option( USE_PULSE "Enable pulseaudio support" OFF )
	option( USE_QT5 "Use Qt5" ON )
	option( BUILD_TRANSLATIONS "Build project translations" ON )
	option( USE_KDE "Enable Kde Notifications support" OFF )
	find_package( ALSA REQUIRED )
	if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" OR ( "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo" ) )
		add_definitions( -DISDEBUG )
		set(CMAKE_CXX_FLAGS_DEBUG "-O0 ${CMAKE_CXX_FLAGS_DEBUG}")
		set(CMAKE_C_FLAGS_DEBUG "-O0 ${CMAKE_C_FLAGS_DEBUG}")
	endif()
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
	include_regular_expression("\\.h$")
	set( CPP_FILES
		main.cpp
		gui/popupwindow.cpp
		alsawork/alsawork.cpp
		alsawork/mixerswitches.cpp
		alsawork/alsadevice.cpp
		gui/settingsdialog.cpp
		gui/trayicon.cpp
	)
	set( UI_FILES
		gui/settingsdialog.ui
	)
	set( QRC_FILES
		icons.qrc
	)
	set( TRANSLATIONS_PATH languages )
	set( DATADIR_PATH ${CMAKE_INSTALL_PREFIX}/share )
	set( APPDIR_PATH ${DATADIR_PATH}/applications )
	set( DESKTOP_FILES ${PROJECT_NAME}.desktop )
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
		${TRANSLATIONS_PATH}/${PROJECT_NAME}_en.ts
		${TRANSLATIONS_PATH}/${PROJECT_NAME}_ru.ts
		${TRANSLATIONS_PATH}/${PROJECT_NAME}_uk.ts
	)
	if( ${USE_PULSE} )
		find_package( PulseAudio REQUIRED )
		include_directories( ${PULSEAUDIO_INCLUDE_DIR} )
		add_definitions( -DUSE_PULSE )
		list(APPEND HEADERS
			pulsework/pulsecore.h
			pulsework/pulsedevice.h
		)
		list(APPEND CPP_FILES
			pulsework/pulsecore.cpp
			pulsework/pulsedevice.cpp
		)
		list(APPEND DEPLIBS
			${PULSEAUDIO_LIBRARY}
		)
	endif()
	if( ${USE_QT5} )
		find_package ( Qt5 COMPONENTS Widgets REQUIRED )
		set( CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}" )
		add_definitions( -DHAVE_QT5 )
		qt5_add_resources( RSCS ${QRC_FILES} )
		if( ${BUILD_TRANSLATIONS} )
			find_package( Qt5LinguistTools REQUIRED )
			qt5_add_translation( TRS ${qtalsavolume_TRS} )
		endif()
		qt5_wrap_ui( UIS ${UI_FILES} )
		list(APPEND DEPLIBS
			Qt5::Widgets
		)
		if( ${USE_KDE} )
			add_definitions( -DKDE_5 -DHAVE_KDE )
			find_package( KF5Notifications REQUIRED )
			find_package( Qt5 COMPONENTS DBus REQUIRED )
			if( KF5Notifications_FOUND )
				message(STATUS "Found KF5::Notifications: (found version \"${KF5Notifications_VERSION}\")")
			endif()
			list(APPEND DEPLIBS
				KF5::Notifications
				Qt5::DBus
			)
		endif()
	else( ${USE_QT5} )
		find_package( Qt4 COMPONENTS QtGui QtCore REQUIRED )
		if( ${USE_KDE} )
			find_package( Qt4 COMPONENTS QtDBus REQUIRED )
		endif()
		include( ${QT_USE_FILE} )
		QT4_WRAP_UI( UIS ${UI_FILES} )
		QT4_ADD_RESOURCES( RSCS ${QRC_FILES} )
		if( ${BUILD_TRANSLATIONS} )
			QT4_ADD_TRANSLATION( TRS ${qtalsavolume_TRS} )
		endif()
		list(APPEND DEPLIBS
			Qt4::QtCore
			Qt4::QtGui
		)
		if( ${USE_KDE} )
			find_package( KDE4 REQUIRED )
			if( KDE4_FOUND )
				message(STATUS "Found KDE4: (found version \"${KDE4_VERSION}\")")
			endif()
			add_definitions( -DKDE_4
					-DHAVE_KDE
					${KDE4_DEFINITIONS}
			)
			include_directories( ${KDE4_INCLUDE_DIRS} )
			list(APPEND DEPLIBS
				${KDE4_KDEUI_LIBS}
				Qt4::QtDBus
			)
			message(STATUS "${KDE4_KDEUI_LIBS}" )
		endif()
	endif()
	add_executable( ${PROJECT_NAME} ${CPP_FILES} ${UIS} ${RSCS} ${TRS} ${MOCS} "defines.h")
	target_link_libraries(
		${PROJECT_NAME}
		${DEPLIBS}
	)
	INSTALL( TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
	INSTALL( FILES ${DESKTOP_FILES} DESTINATION ${APPDIR_PATH} )
	INSTALL( FILES ${ICON_FILES} DESTINATION ${DATADIR_PATH}/${PROJECT_NAME}/icons )
	INSTALL( FILES ${DARK_ICONS} DESTINATION ${DATADIR_PATH}/${PROJECT_NAME}/icons/dark )
	INSTALL( FILES ${LIGHT_ICONS} DESTINATION ${DATADIR_PATH}/${PROJECT_NAME}/icons/light )
	INSTALL( FILES ${TRS} DESTINATION ${DATADIR_PATH}/${PROJECT_NAME}/${TRANSLATIONS_PATH} )
endif()
