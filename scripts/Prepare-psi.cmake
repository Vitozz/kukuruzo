cmake_minimum_required (VERSION 3.10.0)

set(PSI_URL_PREFIX "https://github.com/psi-im")
set(PSI_URL "${PSI_URL_PREFIX}/psi.git")
set(PLUGINS_URL "${PSI_URL_PREFIX}/plugins")
set(TRANSLATIONS_URL "${PSI_URL_PREFIX}/psi-l10n.git")
set(PSIMEDIA_URL "${PSI_URL_PREFIX}/psimedia.git")
set(BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/build" CACHE STRING "Path to build dir")
set(PSI_SRC_DIR "${BUILD_DIR}/psi-build-src")

find_package(Git REQUIRED)

message(STATUS "CURRENT DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}")

if(EXISTS "${PSI_SRC_DIR}")
    message(STATUS "Removing directory: ${PSI_SRC_DIR}")
    file(REMOVE_RECURSE ${PSI_SRC_DIR})
endif()

if(NOT EXISTS "${BUILD_DIR}")
    message(STATUS "Creating directory: ${BUILD_DIR}")
    file(MAKE_DIRECTORY ${BUILD_DIR})
endif()

macro(FetchSRC _NAME _URL _OUTPATH)
    if(EXISTS "${_OUTPATH}")
        message(STATUS "Cleaning existing ${_NAME} git sources...")
        execute_process(
            COMMAND ${GIT_EXECUTABLE} reset --hard
            COMMAND ${GIT_EXECUTABLE} clean -xfd
            WORKING_DIRECTORY ${_OUTPATH}
        )
        message(STATUS "Updating existing ${_NAME} git sources...")
        execute_process(
            COMMAND ${GIT_EXECUTABLE} pull
            WORKING_DIRECTORY ${_OUTPATH}
        )
        execute_process(
            COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
            WORKING_DIRECTORY ${_OUTPATH}
        )
    else()
        message(STATUS "Cloning ${_NAME} git sources...")
        execute_process(
            COMMAND ${GIT_EXECUTABLE} clone ${_URL} ${_OUTPATH}
            WORKING_DIRECTORY ${BUILD_DIR}
        )
        message(STATUS "Cloning ${_NAME} git submodules...")
        execute_process(
            COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
            WORKING_DIRECTORY ${_OUTPATH}
        )
    endif()
endmacro()
macro(Copy_SRC _INPUT_PATH _OUTPUT_PATH)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${_INPUT_PATH} ${_OUTPUT_PATH}
        WORKING_DIRECTORY ${BUILD_DIR}
    )
    message(STATUS "Copy ${_INPUT_PATH} to  ${_OUTPUT_PATH}")
endmacro()
#GIT_EXECUTABLE
message(STATUS "Preparing psi sources...")
FetchSRC(Psi ${PSI_URL} "${BUILD_DIR}/psi")
set(PSI_TAG "")
execute_process( 
    COMMAND ${GIT_EXECUTABLE} describe --tags
    WORKING_DIRECTORY "${BUILD_DIR}/psi"
    OUTPUT_VARIABLE PSI_TAG
)
string(REGEX MATCH "([0-9].[0-9])-.*" _PSI_VER ${PSI_TAG})
if(CMAKE_MATCH_1)
    set(PSI_VER ${CMAKE_MATCH_1})
    execute_process( 
        COMMAND ${GIT_EXECUTABLE} rev-list --count ${PSI_VER}..HEAD
        WORKING_DIRECTORY "${BUILD_DIR}/psi"
        OUTPUT_VARIABLE PSI_REVISION
    )
else()
    message(FATAL_ERROR "Can't obtain psi version")
endif()
if(PSI_VER AND PSI_REVISION)
    set(PSI_VERSION "${PSI_VER}.${PSI_REVISION}")
    message(STATUS "Psi version: ${PSI_VERSION}")
    file(WRITE "${BUILD_DIR}/version.txt" ${PSI_VERSION})
endif()
message(STATUS "Preparing psi plugins sources...")
FetchSRC(Plugins ${PLUGINS_URL} "${BUILD_DIR}/plugins")
if(EXISTS "${BUILD_DIR}/psi")
    Copy_SRC("${BUILD_DIR}/psi" ${PSI_SRC_DIR})
endif()
if(EXISTS "${BUILD_DIR}/plugins")
    Copy_SRC("${BUILD_DIR}/plugins" "${PSI_SRC_DIR}/plugins")
endif()
FetchSRC(Translations ${TRANSLATIONS_URL} "${BUILD_DIR}/translations")
if(EXISTS "${BUILD_DIR}/translations")
    Copy_SRC("${BUILD_DIR}/translations/translations" "${PSI_SRC_DIR}/translations")
endif()
FetchSRC(Psimeida ${PSIMEDIA_URL} "${BUILD_DIR}/psimedia")
if(EXISTS "${BUILD_DIR}/psimedia")
    Copy_SRC("${BUILD_DIR}/psimedia" "${PSI_SRC_DIR}/plugins/generic/psimedia")
endif()
