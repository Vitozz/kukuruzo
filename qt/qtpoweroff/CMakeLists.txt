project( qtpoweroff LANGUAGES CXX)
cmake_minimum_required( VERSION 3.1.0 )
if ( NOT CMAKE_VERSION VERSION_LESS 3.10 )
	cmake_policy(SET CMP0071 OLD)
endif ()
set( CMAKE_AUTOMOC ON )
set( CMAKE_INCLUDE_CURRENT_DIR ON )
option( BUILD_TRANSLATIONS "Build project translations" ON )
option( BUILD_STATIC "Build program with static linking" OFF )
option( USE_CPACK "Enable CPack" OFF )
option( INSTALL_RULES "Install polkit-rules file at Linux" OFF )

if(USE_CPACK)
    include(version.cmake)
    include(use_cpack.cmake)
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)
set( qtpoweroff_SRC
    main.cpp
    mainwindow.cpp
)

set( qtpoweroff_HDR
    mainwindow.h
    defines.h
)
set( qtpoweroff_UI
    mainwindow.ui
)
set( qtpoweroff_QRC
    ${PROJECT_NAME}.qrc
)
set( qtpoweroff_RC
    ${PROJECT_NAME}.rc
)
set( LANG_PATH languages )
set( qtpoweroff_TRS
    ${LANG_PATH}/${PROJECT_NAME}_ru.ts
    ${LANG_PATH}/${PROJECT_NAME}_uk.ts
)
set( DOC_FILES
    README
)
set(LICENSE_FILES
    COPYING
)
if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" OR ( "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo" ) )
    set(DEBUG_ENABLED ON)
    add_definitions( -DIS_DEBUG )
    set( CMAKE_CXX_FLAGS_DEBUG "-O0 ${CMAKE_CXX_FLAGS_DEBUG}" )
endif()

find_package( Qt5 COMPONENTS Widgets REQUIRED )
set( CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}" )
add_definitions( -DHAVE_QT5 )
qt5_add_resources( RSCS ${qtpoweroff_QRC} )
if( ${BUILD_TRANSLATIONS} )
    find_package( Qt5 COMPONENTS LinguistTools REQUIRED )
    qt5_add_translation( TRS ${qtpoweroff_TRS} )
endif()
qt5_wrap_ui( UIS ${qtpoweroff_UI} )
set( QT_DEPLIBS
    Qt5::Widgets
)
if( UNIX AND NOT (APPLE OR CYGWIN) )
    find_package( Qt5 COMPONENTS DBus REQUIRED )
    include_directories( ${Qt5DBus_INCLUDES} )
    add_definitions(
        ${Qt5DBus_DEFINITIONS}
        -DHAVE_DBUS
    )
    list( APPEND QT_DEPLIBS
        Qt5::DBus
    )
endif()
if( UNIX AND NOT (APPLE OR CYGWIN) )
    if("${CMAKE_INSTALL_PREFIX}" STREQUAL "/")
        set(CPACK_SUFFIX "usr")
    endif()
    set( DATADIR_PATH ${CMAKE_INSTALL_PREFIX}${CPACK_SUFFIX}/share )
    set( APPDATA_PATH ${DATADIR_PATH}/applications )
    set( ICON_FILES
        images/32x32.png
        images/48x48.png
        images/72x72.png
        images/128x128.png
    )
    set( ICON_PATH
        ${DATADIR_PATH}/${PROJECT_NAME}/images
    )
    set( DESKTOP_FILES
        ${PROJECT_NAME}.desktop
    )
    set ( POLKIT_RULES
        10-upower.rules
    )
    set ( DOCS_PATH
        ${DATADIR_PATH}/doc/${PROJECT_NAME}
    )
    add_executable( ${PROJECT_NAME}
        ${qtpoweroff_SRC}
        ${UIS}
        ${RSCS}
        ${TRS}
        "defines.h"
    )
    target_link_libraries ( ${PROJECT_NAME} ${QT_DEPLIBS} )
    INSTALL( TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}${CPACK_SUFFIX}/bin )
    INSTALL( FILES ${ICON_FILES} DESTINATION ${ICON_PATH} )
    INSTALL( FILES ${DESKTOP_FILES} DESTINATION ${APPDATA_PATH} )
    INSTALL( FILES ${DOC_FILES} DESTINATION ${DOCS_PATH} )
    INSTALL( FILES ${TRS} DESTINATION ${DATADIR_PATH}/${PROJECT_NAME}/${LANG_PATH} )
    if(INSTALL_RULES)
        INSTALL( FILES ${POLKIT_RULES} DESTINATION ${CMAKE_INSTALL_PREFIX}etc/polkit-1/rules.d )
    endif()
    if(USE_CPACK AND DPKG_PATH)
        INSTALL( FILES ${PROJECT_BINARY_DIR}/copyright DESTINATION ${DATADIR_PATH}/doc/${PROJECT_NAME} )
    endif()
endif()
if( ${WIN32} )
    if( NOT DEBUG_ENABLED )
        add_definitions( -DQT_NO_DEBUG )
    endif()
    if( ${BUILD_STATIC} )
        set( START_STATIC -Wl,-Bstatic )
        set( END_STATIC -Wl,-Bdynamic ws2_32 winmm imm32 )
        list( APPEND CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++" )
    endif()
    set ( CMAKE_BUILD_WITH_INSTALL_RPATH FALSE )
    add_executable( ${PROJECT_NAME}
        WIN32
        ${qtpoweroff_SRC}
        ${UIS}
        ${RSCS}
        ${TRS}
        ${qtpoweroff_RC}
        "defines.h"
    )
    target_link_libraries ( ${PROJECT_NAME}
                ${START_STATIC}
                ${QT_DEPLIBS}
                ${END_STATIC}
    )
    INSTALL( TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX} )
    INSTALL( FILES ${TRS} DESTINATION ${CMAKE_INSTALL_PREFIX}/${LANG_PATH} )
    INSTALL( FILES ${DOC_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX} )
    INSTALL( FILES ${LICENSE_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX} )
endif()
