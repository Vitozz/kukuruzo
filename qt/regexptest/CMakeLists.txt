cmake_minimum_required( VERSION 3.12.0 )
project( regexptest LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules" )

include(policyRules)

set( CMAKE_AUTOMOC ON )
set( CMAKE_INCLUDE_CURRENT_DIR ON )
option( BUILD_TRANSLATIONS "Build project translations" ON )
option( USE_CPACK "Enable CPack" OFF )

include(qtvars)
include(GNUInstallDirs)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)
set( regexptest_SRC
    main.cpp
    regexpwindow.cpp
    syntaxhighlighter.cpp
)

set( regexptest_HDR
    regexpwindow.h
    syntaxhighlighter.h
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

find_qt_package( Widgets REQUIRED )
set(CMAKE_CXX_STANDARD 11)
add_qt_resources( RSCS ${regexptest_QRC} )
if( ${BUILD_TRANSLATIONS} )
    find_qt_package( LinguistTools REQUIRED )
    add_qt_translation( TRS "${regexptest_TRS}" )
    if(USE_QT6)
        qt_add_lupdate(TS_FILES ${regexptest_TRS} SOURCES ${regexptest_SRC} ${regexptest_UI} ${regexptest_HDR} LUPDATE_TARGET lupdate OPTIONS  -no-obsolete -recursive )
    endif()
endif( ${BUILD_TRANSLATIONS} )
wrap_qt_ui( UIS ${regexptest_UI} )
add_qt_library(QT_DEPLIBS Widgets)

if( UNIX AND NOT (APPLE OR CYGWIN) )
    set( DATADIR_PATH ${CMAKE_INSTALL_DATAROOTDIR} )
    set( APPDATA_PATH ${DATADIR_PATH}/applications )
    set( ICON_INSTALL_PREFIX ${DATADIR_PATH}/icons/hicolor)
    set( ICON_FILES_PREFIX "icons")
    set( ICON_PATH
        ${DATADIR_PATH}/${PROJECT_NAME}/icons
    )
    set(PREFIX "${CMAKE_INSTALL_PREFIX}")
    set( DESKTOP_FILES
        ${PROJECT_NAME}.desktop
    )
    configure_file(${DESKTOP_FILES}.in "${PROJECT_BINARY_DIR}/${DESKTOP_FILES}" COPYONLY)
    add_executable( ${PROJECT_NAME}
        ${regexptest_SRC}
        ${UIS}
        ${RSCS}
        ${TRS}
    )
    target_link_libraries ( ${PROJECT_NAME} ${QT_DEPLIBS} )
    INSTALL( TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} )
    set(RESOLUTIONS 16 32 48 64 128)
    foreach(resolution ${RESOLUTIONS})
        unset(LOGO_NAME)
        set(LOGO_NAME "${ICON_FILES_PREFIX}/${resolution}x${resolution}.png")
        install(FILES ${LOGO_NAME} DESTINATION "${ICON_INSTALL_PREFIX}/${resolution}x${resolution}/apps" RENAME ${PROJECT_NAME}.png)
    endforeach()
    INSTALL( FILES "${PROJECT_BINARY_DIR}/${DESKTOP_FILES}" DESTINATION ${APPDATA_PATH} )
    INSTALL( FILES ${DOC_FILES} DESTINATION ${CMAKE_INSTALL_DOCDIR}/html )
    INSTALL( FILES ${TRS} DESTINATION ${DATADIR_PATH}/${PROJECT_NAME}/${LANG_PATH} )
    if(USE_CPACK AND DPKG_PATH)
        INSTALL( FILES ${PROJECT_BINARY_DIR}/copyright DESTINATION ${CMAKE_INSTALL_DOCDIR} )
    endif()
endif()
if( WIN32 )
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
if(USE_CPACK)
    include(version)
    #CPack section start
    include(use_cpack)
    #CPack section end
endif()
