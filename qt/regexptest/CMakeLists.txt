project( regexptest )
cmake_minimum_required( VERSION 3.1.0 )
if ( NOT CMAKE_VERSION VERSION_LESS 3.10 )
	cmake_policy(SET CMP0071 OLD)
endif ()
set( CMAKE_AUTOMOC ON )
set( CMAKE_INCLUDE_CURRENT_DIR ON )
option( BUILD_TRANSLATIONS "Build project translations" ON )
option( USE_CPACK "Enable CPack" OFF )

if(USE_CPACK)
    include(version.cmake)
    #CPack section start
    include(use_cpack.cmake)
    #CPack section end
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)
set( regexptest_SRC
    main.cpp
    regexpwindow.cpp
)

set( regexptest_HDR
    regexpwindow.h
)
set( regexptest_UI
    regexpwindow.ui
)
set( regexptest_QRC
    resources.qrc
)
set( regexptest_RC
    MyApp.rc
)
set( LANG_PATH langs )
set( regexptest_TRS
    ${LANG_PATH}/${PROJECT_NAME}_ru.ts
    ${LANG_PATH}/${PROJECT_NAME}_uk.ts
)
set( DOC_FILES
    docs/regexp_help.html
)
set(LICENSE_FILES
    COPYING
)
if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" OR ( "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo" ) )
    add_definitions(-DIS_DEBUG)
    set(DEBUG_ENABLED ON)
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 ${CMAKE_CXX_FLAGS_DEBUG}")
endif()

find_package( Qt5 COMPONENTS Widgets REQUIRED )
set(CMAKE_CXX_STANDARD 11)
qt5_add_resources( RSCS ${regexptest_QRC} )
if( ${BUILD_TRANSLATIONS} )
    find_package( Qt5 COMPONENTS LinguistTools REQUIRED )
    qt5_add_translation( TRS ${regexptest_TRS} )
endif( ${BUILD_TRANSLATIONS} )
qt5_wrap_ui( UIS ${regexptest_UI} )
set( QT_DEPLIBS
    Qt5::Widgets
)
if( UNIX AND NOT (APPLE OR CYGWIN) )
    set( DATADIR_PATH ${CMAKE_INSTALL_PREFIX}/share )
    set( APPDATA_PATH ${DATADIR_PATH}/applications )
    set( ICON_FILES
        icons/72x72.png
    )
    set( ICON_PATH
        ${DATADIR_PATH}/${PROJECT_NAME}/icons
    )
    set( DESKTOP_FILES
        ${PROJECT_NAME}.desktop
    )
    set ( DOCS_PATH
        ${DATADIR_PATH}/doc/${PROJECT_NAME}/html
    )
    add_executable( ${PROJECT_NAME}
        ${regexptest_SRC}
        ${UIS}
        ${RSCS}
        ${TRS}
    )
    target_link_libraries ( ${PROJECT_NAME} ${QT_DEPLIBS} )
    INSTALL( TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
    INSTALL( FILES ${ICON_FILES} DESTINATION ${ICON_PATH} )
    INSTALL( FILES ${DESKTOP_FILES} DESTINATION ${APPDATA_PATH} )
    INSTALL( FILES ${DOC_FILES} DESTINATION ${DOCS_PATH} )
    INSTALL( FILES ${TRS} DESTINATION ${DATADIR_PATH}/${PROJECT_NAME}/${LANG_PATH} )
    if(USE_CPACK AND DPKG_PATH)
        INSTALL( FILES ${PROJECT_BINARY_DIR}/copyright DESTINATION ${DATADIR_PATH}/doc/${PROJECT_NAME} )
    endif()
endif()
if( ${WIN32} )
    if( NOT DEBUG_ENABLED )
        add_definitions( -DQT_NO_DEBUG -pthread )
    endif()
    set ( CMAKE_BUILD_WITH_INSTALL_RPATH FALSE )
    add_executable( ${PROJECT_NAME}
        WIN32
        ${regexptest_SRC}
        ${UIS}
        ${RSCS}
        ${TRS}
        ${regexptest_RC}
    )
    target_link_libraries ( ${PROJECT_NAME}
                ${QT_DEPLIBS}
    )
    INSTALL( TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX} )
    INSTALL( FILES ${TRS} DESTINATION ${CMAKE_INSTALL_PREFIX}/langs )
    INSTALL( FILES ${DOC_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/docs )
    INSTALL( FILES ${LICENSE_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX} )
endif()
