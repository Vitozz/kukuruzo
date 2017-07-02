project ( ssdisable )
cmake_minimum_required( VERSION 2.8.12 )

subdirs (src)

set ( DESKTOP_FILES
	${PROJECT_NAME}.desktop
)

set ( DATADIR_PATH
	${CMAKE_INSTALL_PREFIX}/share
)
set ( APPDATADIR_PATH
	${DATADIR_PATH}/applications
)

INSTALL ( FILES ${DESKTOP_FILES} DESTINATION ${APPDATADIR_PATH} )
