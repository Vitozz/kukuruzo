set (PROJECT qtalsavolume )
cmake_minimum_required( VERSION 2.6 )
add_definitions ( -Wall )

find_package ( Qt4 REQUIRED )
include ( ${QT_USE_FILE} )

find_package ( PkgConfig REQUIRED )
pkg_check_modules ( ALSA REQUIRED alsa )

link_directories ( ${ALSA_LIBRARY_DIRS} )
include_directories (
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR}
	${QT_QTGUI_INCLUDE_DIR}
	${QT_QTCORE_INCLUDE_DIR}
	${QT_QTGUI_INCLUDE_DIR}
	${ALSA_INCLUDE_DIRS}
)
set ( HEADERS
	alsawork/alsawork.h
	alsawork/mixerswitches.h
	alsawork/volumemixers.h
	defines.h
)

set ( CPP_FILES
	main.cpp
	popupwindow.cpp
	alsawork/alsawork.cpp
	alsawork/mixerswitches.cpp
	alsawork/volumemixers.cpp
	settingsdialog.cpp
)

set ( UI_FILES
	settingsdialog.ui
)
QT4_WRAP_UI(UIS ${UI_FILES})

set ( QRC_FILES
	icons.qrc
)
QT4_ADD_RESOURCES(RSCS ${QRC_FILES})

set ( TRANSLATIONS_PATH languages )
set ( DATADIR_PATH share )
set ( APPDIR_PATH ${DATADIR_PATH}/applications )
set ( DESKTOP_FILES ${PROJECT}.desktop )
set ( ICON_FILES icons/volume_ico.png )
set ( DARK_ICONS
	icons/dark/tb_icon0.png
	icons/dark/tb_icon20.png
	icons/dark/tb_icon40.png
	icons/dark/tb_icon60.png
	icons/dark/tb_icon80.png
	icons/dark/tb_icon100.png
)
set ( LIGHT_ICONS
	icons/light/tb_icon0.png
	icons/light/tb_icon20.png
	icons/light/tb_icon40.png
	icons/light/tb_icon60.png
	icons/light/tb_icon80.png
	icons/light/tb_icon100.png
)

set ( qtalsavolume_TRS
	${TRANSLATIONS_PATH}/${PROJECT}_en.ts
	${TRANSLATIONS_PATH}/${PROJECT}_ru.ts
	${TRANSLATIONS_PATH}/${PROJECT}_uk.ts
)

QT4_ADD_TRANSLATION(TRS ${qtalsavolume_TRS})

set ( MOC_FILES
	popupwindow.h
	settingsdialog.h
)
QT4_WRAP_CPP(MOCS ${MOC_FILES})

add_executable ( ${PROJECT} ${CPP_FILES} ${UIS} ${RSCS} ${TRS} ${MOCS} )
target_link_libraries (
	${PROJECT}
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${ALSA_LIBRARIES}
)
INSTALL ( TARGETS ${PROJECT} RUNTIME DESTINATION bin )
INSTALL ( FILES ${DESKTOP_FILES} DESTINATION ${APPDIR_PATH} )
INSTALL ( FILES ${ICON_FILES} DESTINATION ${DATADIR_PATH}/${PROJECT}/icons )
INSTALL ( FILES ${DARK_ICONS} DESTINATION ${DATADIR_PATH}/${PROJECT}/icons/dark )
INSTALL ( FILES ${LIGHT_ICONS} DESTINATION ${DATADIR_PATH}/${PROJECT}/icons/light )
INSTALL ( FILES ${TRS} DESTINATION ${DATADIR_PATH}/${PROJECT}/${TRANSLATIONS_PATH} )