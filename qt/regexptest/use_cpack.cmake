cmake_minimum_required( VERSION 3.1.0 )

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
    find_program(MAKEPKG makepkg DOC "Path to makepkg")
    set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
    if(RPMB_PATH)
        set(CPACK_GENERATOR "RPM")
        set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")
        set(CPACK_RPM_PACKAGE_LICENSE "GPL-3")
        set(CPACK_RPM_PACKAGE_GROUP "Development/Tools")
        set(CPACK_RESOURCE_FILE_LICENSE "${CPACK_RESOURCE_FILE_LICENSE}")
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
        set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${PACKAGE_MAINTAINER}")
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
    if(MAKEPKG)
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
