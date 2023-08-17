cmake_minimum_required( VERSION 3.5.0 )
project(qtalsavolume LANGUAGES CXX)
if ( NOT CMAKE_VERSION VERSION_LESS 3.10 )
    cmake_policy(SET CMP0071 NEW)
endif ()
if( UNIX AND NOT (APPLE OR CYGWIN) )
    include(qtvars.cmake)
    set( CMAKE_AUTOMOC ON )
    set( CMAKE_INCLUDE_CURRENT_DIR ON )
    set(CMAKE_CXX_STANDARD 11)
    option( USE_PULSE "Enable pulseaudio support" OFF )
    option( BUILD_TRANSLATIONS "Build project translations" ON )
    option( USE_KDE "Enable Kde Notifications support" OFF )
    option( USE_CPACK "Enable CPack" OFF )

    #CPack section start
    if(USE_CPACK)
        include(version.cmake)
        include(use_cpack.cmake)
    #CPack section end
    endif()
    include(fix-codestyle.cmake)

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
    set( qtalsavolume_TRS
        ${TRANSLATIONS_PATH}/${PROJECT_NAME}_en.ts
        ${TRANSLATIONS_PATH}/${PROJECT_NAME}_ru.ts
        ${TRANSLATIONS_PATH}/${PROJECT_NAME}_uk.ts
    )
    if( USE_PULSE )
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
    find_qt_package(Widgets REQUIRED)
    if (Qt${QT_PKG_VER}Widgets_FOUND)
        message(STATUS "Qt${QT_PKG_VER} version: ${Qt${QT_PKG_VER}Widgets_VERSION}")
    endif()
    add_qt_resources( RSCS ${QRC_FILES} )
    if( BUILD_TRANSLATIONS )
        find_qt_package(LinguistTools REQUIRED)
        add_qt_translation(TRS "${qtalsavolume_TRS}")
    endif()
    wrap_qt_ui( UIS ${UI_FILES} )
    add_qt_library(DEPLIBS Widgets)

    if( USE_KDE )
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
    set(PREFIX "${CMAKE_INSTALL_PREFIX}")
    configure_file(${DESKTOP_FILES}.in "${PROJECT_BINARY_DIR}/${DESKTOP_FILES}" @ONLY)
    INSTALL( TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
    INSTALL( FILES "${PROJECT_BINARY_DIR}/${DESKTOP_FILES}" DESTINATION ${APPDIR_PATH} )
    INSTALL( FILES ${ICON_FILES} DESTINATION ${DATADIR_PATH}/${PROJECT_NAME}/icons )
    INSTALL( FILES ${TRS} DESTINATION ${DATADIR_PATH}/${PROJECT_NAME}/${TRANSLATIONS_PATH} )
    if(USE_CPACK AND DPKG_PATH)
        INSTALL( FILES ${PROJECT_BINARY_DIR}/copyright DESTINATION ${DATADIR_PATH}/doc/${PROJECT_NAME} )
    endif()
endif()
