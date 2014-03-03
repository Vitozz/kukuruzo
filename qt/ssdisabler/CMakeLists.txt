project ( ssdisable )
cmake_minimum_required( VERSION 2.6 )

subdirs ( 
	src 
)
set ( DESKTOP_FILES 
	${PROJECT_NAME}.desktop
)

set ( APPDATADIR_PATH 
	share/applications
)

INSTALL ( FILES ${DESKTOP_FILES} DESTINATION ${APPDATADIR_PATH} )