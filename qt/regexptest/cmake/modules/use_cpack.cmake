cmake_minimum_required( VERSION 3.12.0 )

#CPack section start
set(CPACK_PACKAGE_VERSION ${_VERSION_STRING})
set(CPACK_PACKAGE_VERSION_MAJOR "${_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${_VERSION_MINOR}")
if(_VERSION_PATCH)
    set(CPACK_PACKAGE_VERSION_PATCH "${_VERSION_PATCH}")
endif()
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_RELEASE 1)
set(CPACK_PACKAGE_CONTACT "Vitaly Tonkacheyev")
set(CPACK_MAINTAINER_EMAIL "thetvg@gmail.com")
set(PACKAGE_MAINTAINER "${CPACK_PACKAGE_CONTACT} <${CPACK_MAINTAINER_EMAIL}>")
set(CPACK_PACKAGE_VENDOR "KukuRuzo Inc")
set(CPACK_PACKAGE_DESCRIPTION "RegExp Tester")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Simple tool written on Qt to test regular expressions")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/COPYING")
set(CPACK_SOURCE_GENERATOR "TGZ")
set(PACKAGE_URL "https://sourceforge.net/projects/kukuruzo/files/regexptest/")
set(_CPACK_GENERATORS)
if(WIN32 AND NOT UNIX)
    list(APPEND _CPACK_GENERATORS "NSIS")
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
    find_program(MAKEPKG makepkg DOC "Path to makepkg")
    set(HOMEDIR "$ENV{HOME}")
    find_program(CPACK_APPIMAGE_TOOL_EXECUTABLE "${HOMEDIR}/AppImages/appimagetool.appimage" DOC "Path to appimagetool")
    find_program(CPACK_APPIMAGE_PATCHELF_EXECUTABLE patchelf DOC "Path to patchelf")
    set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
    if(RPMB_PATH)
        list(APPEND _CPACK_GENERATORS "RPM")
        set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")
        set(CPACK_RPM_PACKAGE_LICENSE "GPL-3")
        set(CPACK_RPM_PACKAGE_GROUP "Development/Tools")
        set(CPACK_RESOURCE_FILE_LICENSE "${CPACK_RESOURCE_FILE_LICENSE}")
        set(CPACK_RPM_PACKAGE_URL "${PACKAGE_URL}")
    endif()
    if(DPKG_PATH)
        list(APPEND _CPACK_GENERATORS "DEB")
        set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${PACKAGE_MAINTAINER}")
        set(CPACK_DEBIAN_PACKAGE_SECTION "x11")
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
            if(NOT "${OSNAME}" STREQUAL "")
                message(STATUS "Current system: ${OSNAME}")
                if("${OSNAME}" STREQUAL "Ubuntu")
                    set(PKG_OS_SUFFIX "-0ubuntu1~0ppa${CPACK_PACKAGE_RELEASE}~")
                endif()
            endif()
            execute_process(COMMAND "${LSB_APP} -cs"
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
            set(CPACK_PACKAGE_ARCHITECTURE "${CMAKE_SYSTEM_PROCESSOR}")
        else()
            set(CPACK_PACKAGE_ARCHITECTURE "${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
        endif()
        if(NOT CPACK_PACKAGE_FILE_NAME)
            set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_DEBIAN_PACKAGE_VERSION}_${CPACK_PACKAGE_ARCHITECTURE}")
        endif()
        configure_file(copyright.in copyright @ONLY)
        set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_BINARY_DIR}/copyright")
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
Plugins = ${CMAKE_INSTALL_PREFIX}/lib/qt${QT_PKG_VER}/plugins
Translations = ${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}/languages
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
            # Path to Qt plugins
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
    if(NOT "${MAKEPKG}" STREQUAL "MAKEPKG-NOTFOUND")
        if(USE_QT6)
            set(QT6_FLAG ON)
        else()
            set(QT6_FLAG OFF)
        endif()
        configure_file(PKGBUILD.in PKGBUILD @ONLY)
        message(STATUS "PKGBUILD for ArchLinux generated in ${PROJECT_BINARY_DIR}")
    endif()
endif()
include(CPack)
#CPack section end
