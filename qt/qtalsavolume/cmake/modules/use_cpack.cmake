cmake_minimum_required( VERSION 3.10.0 )

#CPack section start
set(CPACK_PACKAGE_VERSION ${_VERSION_STRING})
set(CPACK_PACKAGE_VERSION_MAJOR "${_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${_VERSION_MINOR}")
if(_VERSION_PATCH)
    set(CPACK_PACKAGE_VERSION_PATCH "${_VERSION_PATCH}")
endif()
set(_CPACK_PKG_NAME "${PROJECT_NAME}")
if(USE_PULSE)
    set(_CPACK_PKG_NAME "${_CPACK_PKG_NAME}-pulse")
endif()
set(CPACK_PACKAGE_NAME "${_CPACK_PKG_NAME}")
set(CPACK_PACKAGE_RELEASE 1)
set(CPACK_PACKAGE_CONTACT "Vitaly Tonkacheyev")
set(CPACK_MAINTAINER_EMAIL "thetvg@gmail.com")
set(PACKAGE_MAINTAINER "${CPACK_PACKAGE_CONTACT} <${CPACK_MAINTAINER_EMAIL}>")
set(CPACK_PACKAGE_VENDOR "KukuRuzo Inc")
set(CPACK_PACKAGE_DESCRIPTION "Tray ALSA volume changer")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Simple tool written using Qt library to set the levels of alsa mixers.")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/COPYING")
set(CPACK_SOURCE_GENERATOR "TGZ")
set(PACKAGE_URL "https://sourceforge.net/projects/kukuruzo/files/qtalsavolume/")
find_program(RPMB_PATH rpmbuild DOC "Path to rpmbuild")
find_program(DPKG_PATH dpkg-deb DOC "Path to dpkg")
find_program(MAKEPKG makepkg DOC "Path to makepkg")
set(HOMEDIR "$ENV{HOME}")
find_program(CPACK_APPIMAGE_TOOL_EXECUTABLE "${HOMEDIR}/AppImages/appimagetool.appimage" DOC "Path to appimagetool")
find_program(CPACK_APPIMAGE_PATCHELF_EXECUTABLE patchelf DOC "Path to patchelf")
set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set(_CPACK_GENERATORS)
if(RPMB_PATH)
    list(APPEND _CPACK_GENERATORS "RPM")
    set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")
    set(CPACK_RPM_PACKAGE_LICENSE "GPL-3")
    set(CPACK_RPM_PACKAGE_GROUP "Applications/Multimedia")
    set(CPACK_RPM_SPEC_CHANGELOG "${PROJECT_SOURCE_DIR}/ChangeLog")
    set(CPACK_RESOURCE_FILE_LICENSE "${CPACK_RESOURCE_FILE_LICENSE}")
    set(CPACK_RPM_PACKAGE_URL "${PACKAGE_URL}")
    message(STATUS "CPack: RPM generator added")
endif()
if(DPKG_PATH)
    list(APPEND _CPACK_GENERATORS "DEB")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${PACKAGE_MAINTAINER}")
    set(CPACK_DEBIAN_PACKAGE_SECTION "sound")
    execute_process(COMMAND "${DPKG_PATH} --print-architecture"
        OUTPUT_VARIABLE DEB_PKG_ARCH
    )
    if(NOT "${DEB_PKG_ARCH}" STREQUAL "")
        set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${DEB_PKG_ARCH}")
    endif()
    find_program(LSB_APP lsb_release DOC "Path to lsb_release")
    if(LSB_APP)
        execute_process(COMMAND "${LSB_APP} -is"
            OUTPUT_VARIABLE OSNAME
        )
        if(OSNAME)
            message(STATUS "Current system: ${OSNAME}")
            if("${OSNAME}" STREQUAL "Ubuntu")
                set(PKG_OS_SUFFIX "-0ubuntu1~0ppa${CPACK_PACKAGE_RELEASE}~")
            endif()
        endif()
        execute_process(COMMAND "${LSB_APP} -cs"
            OUTPUT_VARIABLE OSCODENAME
        )
        if(OSCODENAME)
            message(STATUS "Debian codename: ${OSCODENAME}")
        endif()
    endif()
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
    if(NOT CPACK_DEBIAN_PACKAGE_VERSION)
        set(CPACK_DEBIAN_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}${PKG_OS_SUFFIX}${OSCODENAME}")
    endif()
    if(NOT CPACK_DEBIAN_PACKAGE_ARCHITECTURE)
        set(CPACK_PACKAGE_ARCHITECTURE "${CMAKE_SYSTEM_PROCESSOR}")
    else()
        set(CPACK_PACKAGE_ARCHITECTURE "${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
    endif()
    if(NOT CPACK_PACKAGE_FILE_NAME)
        set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_DEBIAN_PACKAGE_VERSION}_${CPACK_PACKAGE_ARCHITECTURE}")
    endif()
    configure_file(copyright.in copyright @ONLY)
    set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_BINARY_DIR}/copyright")
    message(STATUS "CPack: DEB generator added")
endif()
if(CPACK_APPIMAGE_TOOL_EXECUTABLE AND CPACK_APPIMAGE_PATCHELF_EXECUTABLE)
    if(CMAKE_VERSION GREATER_EQUAL 4.2.0)
        list(APPEND _CPACK_GENERATORS "AppImage")
        set(CPACK_PACKAGE_ICON "${PROJECT_NAME}.png")
        install(CODE "
    file(GET_RUNTIME_DEPENDENCIES
        EXECUTABLES \"${CMAKE_BINARY_DIR}/${PROJECT_NAME}\"
        RESOLVED_DEPENDENCIES_VAR resolved_deps
        POST_EXCLUDE_REGEXES
            \".*/ld-linux[^/]*\\\\.so.*\"
            \".*/libc\\\\.so.*\"
            \".*/libm\\\\.so.*\"
            \".*/libpthread\\\\.so.*\"
            \".*/libdl\\\\.so.*\"
            \".*/librt\\\\.so.*\"
    )

    foreach(dep \${resolved_deps})
        # copy the symlink
        file(INSTALL DESTINATION \"\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}\" TYPE FILE FILES \${dep})

        # Resolve the real path of the dependency (follows symlinks)
        file(REAL_PATH \${dep} resolved_dep_path)

        # Copy the resolved file to the destination
        file(INSTALL DESTINATION \"\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}\" TYPE FILE FILES \${resolved_dep_path})
    endforeach()
")

        set(QTCONF_TEXT
"
[Paths]
Plugins = ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/qt${QT_PKG_VER}/plugins
"
        )
        file(WRITE "${CMAKE_BINARY_DIR}/qt.conf" "${QTCONF_TEXT}")
            install(
            FILES "${CMAKE_BINARY_DIR}/qt.conf"
            DESTINATION ${CMAKE_INSTALL_BINDIR}
        )
        function(install_qt_plugin PLUG_TYPE_NAME PLUG_SEARCH_PATH)
            file(GLOB _PLUGINS
                "${PLUG_SEARCH_PATH}/${PLUG_TYPE_NAME}/libq*.so"
            )
            message(STATUS "CPack: AppImage ${PLUG_TYPE_NAME} plugins added")
            install(
                FILES
                ${_PLUGINS}
                DESTINATION
                "${CMAKE_INSTALL_LIBDIR}/qt${QT_PKG_VER}/plugins/${PLUG_TYPE_NAME}"
            )
            unset(_PLUGINS)
        endfunction()
        # Путь к установленным Qt-плагинам
        execute_process(
            COMMAND "${QT_HOST_PATH}/bin/qmake${QT_PKG_VER}" -query QT_INSTALL_PLUGINS
            OUTPUT_VARIABLE QT_INSTALL_PLUGINS
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        if(QT_INSTALL_PLUGINS)
            set(_QT_PLUGINS
                imageformats
                iconengines
                platforms
                position
            )
            foreach(plugin ${_QT_PLUGINS})
                install_qt_plugin("${plugin}" "${QT_INSTALL_PLUGINS}")
            endforeach()
        endif()
        message(STATUS "CPack: AppImage generator added")
    endif()
endif()
if(_CPACK_GENERATORS)
    set(CPACK_GENERATOR "${_CPACK_GENERATORS}")
else()
    message(WARNING "USE_CPACK flag is enabled but no generators available")
endif()
if(MAKEPKG)
    if(USE_KDE AND USE_QT6)
        set(KDE_DEP 'knotifications')
    elseif(USE_KDE)
        set(KDE_DEP 'knotifications5')
    endif()
    configure_file(PKGBUILD.in PKGBUILD @ONLY)
    message(STATUS "PKGBUILD for ArchLinux generated in ${PROJECT_BINARY_DIR}")
endif()
include(CPack)
#CPack section end
