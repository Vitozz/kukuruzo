project ( ssdisable )
cmake_minimum_required( VERSION 2.6 )
set ( CMAKE_BUILD_TYPE Release )
add_definitions ( -Wall )
find_package ( Qt4 REQUIRED )
include ( ${QT_USE_FILE} )
include_directories (
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR}
	${QT_QTGUI_INCLUDE_DIR}
	${QT_QTCORE_INCLUDE_DIR}
	${QT_QTGUI_INCLUDE_DIR}
)

set ( PSOURCES
	main.cpp
	ssdisabler.cpp
)

set ( RESOURCES
	ssdisable.qrc
)
QT4_ADD_RESOURCES( RSCS ${RESOURCES} )

set ( TRS_PATH langs )

set ( TRANSLATIONS
	${PROJECT_NAME}_en.ts
	${PROJECT_NAME}_ru.ts
	${PROJECT_NAME}_uk.ts
)
QT4_ADD_TRANSLATION( TRS ${TRANSLATIONS} )

set ( MOCFILES
	ssdisabler.h
)
QT4_WRAP_CPP( MOCS ${MOCFILES} )

add_executable ( ${PROJECT_NAME} ${PSOURCES} ${RSCS} ${TRS} ${MOCS} )
target_link_libraries ( ${PROJECT_NAME}  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} )

set ( ICON_FILES
	icon.png
)

set ( DATADIR_PATH 
	share
)

INSTALL ( TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin )
INSTALL ( FILES ${ICON_FILES} DESTINATION ${DATADIR_PATH}/${PROJECT_NAME}/icons )
INSTALL ( FILES ${TRS} DESTINATION ${DATADIR_PATH}/${PROJECT_NAME}/${TRS_PATH} )