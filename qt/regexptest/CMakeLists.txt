project( regexptest )
cmake_minimum_required( VERSION 2.6 )
add_definitions ( -Wall )
find_package ( Qt4 REQUIRED )
include ( ${QT_USE_FILE} )
include_directories (
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR}
	${QT_QTGUI_INCLUDE_DIR}
	)

set ( SOURCE_FILES
	main.cpp
	mainwindow.cpp
)

set ( UI_FILES
	mainwindow.ui
)

QT4_WRAP_UI(UIS ${UI_FILES})

set ( RESOURCES
	resources.qrc
)

QT4_ADD_RESOURCES(RSCS ${RESOURCES})

set ( LANG_PATH langs )

set ( TRANSLATIONS
	${LANG_PATH}/${PROJECT_NAME}_ru.ts
	${LANG_PATH}/${PROJECT_NAME}_uk.ts
)
QT4_ADD_TRANSLATION(TRS ${TRANSLATIONS})

set ( MOC_FILES
	mainwindow.h
)
QT4_WRAP_CPP(MOCS ${MOC_FILES})

set ( DATADIR_PATH share )
set ( APPDATA_PATH ${DATADIR_PATH}/applications )
set ( ICON_FILES 
	icons/72x72.png
)
set ( ICON_PATH
	${DATADIR_PATH}/${PROJECT_NAME}/icons
)
set ( DESKTOP_FILES
	${PROJECT_NAME}.desktop
)
set ( DOC_FILES
	docs/regexp_help.html
)
set ( DOCS_PATH
	${DATADIR_PATH}/doc/${PROJECT_NAME}/html
)

add_executable  ( ${PROJECT_NAME} ${SOURCE_FILES} ${UIS} ${RSCS} ${TRS} ${MOCS} )
target_link_libraries ( ${PROJECT_NAME} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} )

INSTALL ( TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin )
INSTALL ( FILES ${ICON_FILES} DESTINATION ${ICON_PATH} )
INSTALL ( FILES ${DESKTOP_FILES} DESTINATION ${APPDATA_PATH} )
INSTALL ( FILES ${DOC_FILES} DESTINATION ${DOCS_PATH} )
INSTALL ( FILES ${TRS} DESTINATION ${DATADIR_PATH}/${PROJECT_NAME}/${LANG_PATH} )