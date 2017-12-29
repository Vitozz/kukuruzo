project(qtalsavolume )
cmake_minimum_required( VERSION 2.8.12 )
if( UNIX AND NOT (APPLE OR CYGWIN) )
    set( CMAKE_AUTOMOC ON )
    set( CMAKE_INCLUDE_CURRENT_DIR ON )
    option( USE_PULSE "Enable pulseaudio support" OFF )
    option( BUILD_TRANSLATIONS "Build project translations" ON )
    option( USE_KDE "Enable Kde Notifications support" OFF )
    option( USE_CPACK "Enable CPack" OFF )

    #CPack section start
    if(USE_CPACK)
        set(VER_FILE ${PROJECT_SOURCE_DIR}/defines.h)
        if(EXISTS "${VER_FILE}")
            file(STRINGS "${VER_FILE}" VER_LINES)
            string(REGEX MATCHALL "^.+APP_VERSION.+([0-9]+)\\.([0-9]+)\\.([0-9]+)" VER_LINE_A ${VER_LINES})
            if(${CMAKE_MATCH_COUNT} EQUAL 3)
                set(MAJOR_VERSION "${CMAKE_MATCH_1}")
                set(MINOR_VERSION "${CMAKE_MATCH_2}")
                set(PATCH_VERSION "${CMAKE_MATCH_3}")
            endif()
            message(STATUS "Version: ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}")
        endif()
        set(VERSION "${MAJOR_VERSION}.${MINOR_VERSION}")
        if(NOT "${PATCH_VERSION}" STREQUAL "")
            set(VERSION "${VERSION}.${PATCH_VERSION}")
        endif()
        set(CPACK_PACKAGE_VERSION ${VERSION})
        set(CPACK_PACKAGE_VERSION_MAJOR "${MAJOR_VERSION}")
        set(CPACK_PACKAGE_VERSION_MINOR "${MINOR_VERSION}")
        if(NOT "${PATCH_VERSION}" STREQUAL " ")
            set(CPACK_PACKAGE_VERSION_PATCH "${PATCH_VERSION}")
        endif()
        set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
        set(CPACK_PACKAGE_RELEASE 1)
        set(CPACK_GENERATOR "RPM")
        set(CPACK_PACKAGE_CONTACT "Vitaly Tonkacheyev")
        set(CPACK_PACKAGE_VENDOR "KukuRuzo Inc")
        set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Simple tool written using Qt library to set the levels of alsa mixers")
        set(PACKAGE_URL "https://sourceforge.net/projects/kukuruzo/files/qtalsavolume/")
        set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
        find_program(RPMB_PATH rpmbuild DOC "Path to rpmbuild")
        find_program(DPKG_PATH dpkg DOC "Path to dpkg")
        set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
        if(RPMB_PATH)
            set(CPACK_GENERATOR "RPM")
            set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")
            set(CPACK_RPM_PACKAGE_LICENSE "GPL-3")
            set(CPACK_RPM_PACKAGE_GROUP "Applications/Multimedia")
            set(CPACK_RPM_PACKAGE_DESCRIPTION "Simple tool written using Qt library to set the levels of alsa mixers")
            set(CPACK_RPM_SPEC_CHANGELOG "${PROJECT_SOURCE_DIR}/ChangeLog")
            set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/COPYING")
            set(CPACK_RPM_PACKAGE_URL "${PACKAGE_URL}")
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
            set(CPACK_DEBIAN_PACKAGE_SECTION "sound")
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
            set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_BINARY_DIR}/copyright")
        endif()
        include(CPack)
    #CPack section end
    endif()

    find_package( ALSA REQUIRED )
    if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" OR ( "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo" ) )
        add_definitions( -DISDEBUG )
        set(CMAKE_CXX_FLAGS_DEBUG "-O0 ${CMAKE_CXX_FLAGS_DEBUG}")
        set(CMAKE_C_FLAGS_DEBUG "-O0 ${CMAKE_C_FLAGS_DEBUG}")
    endif()
    set( DEPLIBS
        ${ALSA_LIBRARIES}
    )
    include_directories(
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${ALSA_INCLUDE_DIRS}
    )
    set( HEADERS
        alsawork/alsawork.h
        alsawork/mixerswitches.h
        alsawork/alsadevice.h
        defines.h
    )
    include_regular_expression("\\.h$")
    set( CPP_FILES
        main.cpp
        gui/popupwindow.cpp
        alsawork/alsawork.cpp
        alsawork/mixerswitches.cpp
        alsawork/alsadevice.cpp
        gui/settingsdialog.cpp
        gui/trayicon.cpp
    )
    set( UI_FILES
        gui/settingsdialog.ui
    )
    set( QRC_FILES
        icons.qrc
    )
    set( TRANSLATIONS_PATH languages )
    set( DATADIR_PATH ${CMAKE_INSTALL_PREFIX}/share )
    set( APPDIR_PATH ${DATADIR_PATH}/applications )
    set( DESKTOP_FILES ${PROJECT_NAME}.desktop )
    set( ICON_FILES icons/volume_ico.png )
    set( DARK_ICONS
        icons/dark/tb_icon0.png
        icons/dark/tb_icon10.png
        icons/dark/tb_icon20.png
        icons/dark/tb_icon30.png
        icons/dark/tb_icon40.png
        icons/dark/tb_icon50.png
        icons/dark/tb_icon60.png
        icons/dark/tb_icon70.png
        icons/dark/tb_icon80.png
        icons/dark/tb_icon90.png
        icons/dark/tb_icon100.png
    )
    set( LIGHT_ICONS
        icons/light/tb_icon0.png
        icons/light/tb_icon10.png
        icons/light/tb_icon20.png
        icons/light/tb_icon30.png
        icons/light/tb_icon40.png
        icons/light/tb_icon50.png
        icons/light/tb_icon60.png
        icons/light/tb_icon70.png
        icons/light/tb_icon80.png
        icons/light/tb_icon90.png
        icons/light/tb_icon100.png
    )
    set( qtalsavolume_TRS
        ${TRANSLATIONS_PATH}/${PROJECT_NAME}_en.ts
        ${TRANSLATIONS_PATH}/${PROJECT_NAME}_ru.ts
        ${TRANSLATIONS_PATH}/${PROJECT_NAME}_uk.ts
    )
    if( ${USE_PULSE} )
        find_package( PulseAudio REQUIRED )
        if( PulseAudio_FOUND )
            message(STATUS "Found PulseAudio: (found version \"${PulseAudio_VERSION}\")")
        endif()
        include_directories( ${PULSEAUDIO_INCLUDE_DIR} )
        add_definitions( -DUSE_PULSE )
        list(APPEND HEADERS
            pulsework/pulsecore.h
            pulsework/pulsedevice.h
        )
        list(APPEND CPP_FILES
            pulsework/pulsecore.cpp
            pulsework/pulsedevice.cpp
        )
        list(APPEND DEPLIBS
            ${PULSEAUDIO_LIBRARY}
        )
    endif()
    find_package ( Qt5 COMPONENTS Widgets REQUIRED )
    set( CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}" )
    add_definitions( -DHAVE_QT5 )
    qt5_add_resources( RSCS ${QRC_FILES} )
    if( ${BUILD_TRANSLATIONS} )
        find_package( Qt5LinguistTools REQUIRED )
        qt5_add_translation( TRS ${qtalsavolume_TRS} )
    endif()
    qt5_wrap_ui( UIS ${UI_FILES} )
    list(APPEND DEPLIBS
        Qt5::Widgets
    )
    if( ${USE_KDE} )
        add_definitions( -DKDE_5 -DHAVE_KDE )
        find_package( KF5Notifications REQUIRED )
        find_package( Qt5 COMPONENTS DBus REQUIRED )
        if( KF5Notifications_FOUND )
            message(STATUS "Found KF5::Notifications: (found version \"${KF5Notifications_VERSION}\")")
        endif()
        list(APPEND DEPLIBS
            KF5::Notifications
            Qt5::DBus
        )
    endif()
    add_executable( ${PROJECT_NAME} ${CPP_FILES} ${UIS} ${RSCS} ${TRS} ${MOCS} "defines.h")
    target_link_libraries(
        ${PROJECT_NAME}
        ${DEPLIBS}
    )
    INSTALL( TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
    INSTALL( FILES ${DESKTOP_FILES} DESTINATION ${APPDIR_PATH} )
    INSTALL( FILES ${ICON_FILES} DESTINATION ${DATADIR_PATH}/${PROJECT_NAME}/icons )
    INSTALL( FILES ${DARK_ICONS} DESTINATION ${DATADIR_PATH}/${PROJECT_NAME}/icons/dark )
    INSTALL( FILES ${LIGHT_ICONS} DESTINATION ${DATADIR_PATH}/${PROJECT_NAME}/icons/light )
    INSTALL( FILES ${TRS} DESTINATION ${DATADIR_PATH}/${PROJECT_NAME}/${TRANSLATIONS_PATH} )
    if(USE_CPACK AND DPKG_PATH)
        INSTALL( FILES ${PROJECT_BINARY_DIR}/copyright DESTINATION ${DATADIR_PATH}/doc/${PROJECT_NAME} )
    endif()
endif()
