cmake_minimum_required( VERSION 3.1.0 )

set(CMAKE_AUTOMOC ON)

find_package ( Qt5 COMPONENTS Widgets LinguistTools REQUIRED )

include_directories (
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
)

set ( PSOURCES
	main.cpp
	ssdisabler.cpp
)

set ( RESOURCES
	${PROJECT_NAME}.qrc
)
qt5_add_resources( RSCS ${RESOURCES} )

set ( TRS_PATH langs )

set ( TRANSLATIONS
	${PROJECT_NAME}_en.ts
	${PROJECT_NAME}_ru.ts
	${PROJECT_NAME}_uk.ts
)
qt5_add_translation( TRS ${TRANSLATIONS} )

add_executable ( ${PROJECT_NAME} ${PSOURCES} ${RSCS} ${TRS} )

target_link_libraries ( ${PROJECT_NAME}  Qt5::Widgets )

set ( ICON_FILES
	icon.png
)

INSTALL ( TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
INSTALL ( FILES ${ICON_FILES} DESTINATION ${DATADIR_PATH}/${PROJECT_NAME}/icons )
INSTALL ( FILES ${TRS} DESTINATION ${DATADIR_PATH}/${PROJECT_NAME}/${TRS_PATH} )
