project( regexptest )
cmake_minimum_required( VERSION 2.8.12 )
set( CMAKE_AUTOMOC ON )
set( CMAKE_INCLUDE_CURRENT_DIR ON )
option( BUILD_TRANSLATIONS "Build project translations" ON )
option( USE_CPACK "Enable CPack" OFF )

if(USE_CPACK)
    set(VER_FILE ${PROJECT_SOURCE_DIR}/version.txt)
    if(EXISTS "${VER_FILE}")
        file(STRINGS "${VER_FILE}" VER_LINES)
        string(REGEX MATCHALL "^([0-9]+)\\.([0-9]+)\\.([0-9]+)" VER_LINE_A ${VER_LINES})
        if(${CMAKE_MATCH_COUNT} EQUAL 3)
            set(MAJOR_VERSION "${CMAKE_MATCH_1}")
            if(CMAKE_MATCH_2)
                set(MINOR_VERSION "${CMAKE_MATCH_2}")
            endif()
            if(CMAKE_MATCH_3)
                set(PATCH_VERSION "${CMAKE_MATCH_3}")
            endif()
        endif()
        message(STATUS "Version: ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}")
    endif()


    #CPack section start
    set(VERSION "${MAJOR_VERSION}.${MINOR_VERSION}")
    if(PATCH_VERSION)
        set(VERSION "${VERSION}.${PATCH_VERSION}")
    endif()
    set(CPACK_PACKAGE_VERSION ${VERSION})
    set(CPACK_PACKAGE_VERSION_MAJOR "${MAJOR_VERSION}")
    set(CPACK_PACKAGE_VERSION_MINOR "${MINOR_VERSION}")
    if(PATCH_VERSION)
        set(CPACK_PACKAGE_VERSION_PATCH "${PATCH_VERSION}")
    endif()
    set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
    set(CPACK_PACKAGE_RELEASE 1)
    set(CPACK_PACKAGE_CONTACT "Vitaly Tonkacheyev")
    set(CPACK_PACKAGE_VENDOR "KukuRuzo Inc")
    set(CPACK_PACKAGE_DESCRIPTION "RegExp Tester")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Simple tool written on Qt to test regular expressions")
    set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/COPYING")
    set(CPACK_SOURCE_GENERATOR "TGZ")
    if(WIN32 AND NOT UNIX)
        set(CPACK_GENERATOR NSIS)
        #set(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
        set(CPACK_NSIS_INSTALLED_ICON_NAME "${PROJECT_NAME}.exe")
        set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} RegExpTest")
        set(CPACK_NSIS_HELP_LINK "https:\\\\\\\\github.com\\\\Vitozz\\\\kukuruzo")
        set(CPACK_NSIS_URL_INFO_ABOUT "RegExpTest Sources")
        set(CPACK_NSIS_CONTACT "thetvg@gmail.com")
        set(CPACK_NSIS_MODIFY_PATH ON)
    else()
        find_program(RPMB_PATH rpmbuild DOC "Path to rpmbuild")
        find_program(DPKG_PATH dpkg DOC "Path to dpkg")
        set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
        if(RPMB_PATH)
            set(CPACK_GENERATOR "RPM")
            set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")
            set(CPACK_RPM_PACKAGE_LICENSE "GPL-3")
            set(CPACK_RPM_PACKAGE_GROUP "Development/Tools")
        endif()
        if(DPKG_PATH)
            set(CPACK_GENERATOR "DEB")
            exec_program("LANG=en date +'%a, %d %b %Y %T %z'"
                OUTPUT_VARIABLE BUILD_DATE
            )
            exec_program("date +'%Y'"
                OUTPUT_VARIABLE BUILD_YEAR
            )
            message(STATUS "Build date: ${BUILD_DATE}")
            set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_CONTACT} <thetvg@gmail.com>")
            set(CPACK_DEBIAN_PACKAGE_SECTION "x11")
            exec_program("${DPKG_PATH} --print-architecture"
                OUTPUT_VARIABLE DEB_PKG_ARCH
            )
            if(NOT "${DEB_PKG_ARCH}" STREQUAL "")
                set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${DEB_PKG_ARCH}")
            endif()
            find_program(LSB_APP lsb_release DOC "Path to lsb_release")
            if(LSB_APP)
                exec_program("${LSB_APP} -is"
                    OUTPUT_VARIABLE OSNAME
                )
                if(NOT "${OSNAME}" STREQUAL "")
                    message(STATUS "Current system: ${OSNAME}")
                    if("${OSNAME}" STREQUAL "Ubuntu")
                        set(PKG_OS_SUFFIX "-0ubuntu1~0ppa${CPACK_PACKAGE_RELEASE}~")
                    endif()
                endif()
                exec_program("${LSB_APP} -cs"
                    OUTPUT_VARIABLE OSCODENAME
                )
                if(NOT "${OSCODENAME}" STREQUAL "")
                    message(STATUS "Debian codename: ${OSCODENAME}")
                endif()
            endif()
            set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
            if(NOT CPACK_DEBIAN_PACKAGE_VERSION)
                set(CPACK_DEBIAN_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}${PKG_OS_SUFFIX}${OSCODENAME}")
            endif()
            if(NOT CPACK_DEBIAN_PACKAGE_ARCHITECTURE)
                set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${CMAKE_SYSTEM_PROCESSOR}")
            endif()
            if(NOT CPACK_PACKAGE_FILE_NAME)
                set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_DEBIAN_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
            endif()
            configure_file(copyright.in copyright @ONLY)
            set(CPACK_RESOURCE_FILE_LICENSE "copyright")
        endif()
    endif()
    include(CPack)
#CPack section end
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)
set( regexptest_SRC
    main.cpp
    mainwindow.cpp
)

set( regexptest_HDR
    mainwindow.h
)
set( regexptest_UI
    mainwindow.ui
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
set( CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}" )
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
