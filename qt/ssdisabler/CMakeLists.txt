project ( ssdisabler )
cmake_minimum_required( VERSION 3.1.0 )
set( CMAKE_AUTOMOC ON )
set( CMAKE_INCLUDE_CURRENT_DIR ON )
option( USE_CPACK "Enable CPack" OFF )

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug" OR ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo"))
    add_definitions(-DIS_DEBUG)
endif()

if( USE_CPACK )
    include(version.cmake)
    include(use_cpack.cmake)
endif()

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
set( DOC_FILES
    README
)

INSTALL ( FILES ${DESKTOP_FILES} DESTINATION ${APPDATADIR_PATH} )
INSTALL( FILES ${DOC_FILES} DESTINATION ${DATADIR_PATH}/doc/${PROJECT_NAME} )

if(USE_CPACK AND DPKG_PATH)
    INSTALL( FILES ${PROJECT_BINARY_DIR}/copyright DESTINATION ${DATADIR_PATH}/doc/${PROJECT_NAME} )
endif()
