cmake_minimum_required (VERSION 3.10.0)

set(PSI_URL_PREFIX "https://github.com/psi-im")
set(PSI_URL "${PSI_URL_PREFIX}/psi.git")
set(PLUGINS_URL "${PSI_URL_PREFIX}/plugins.git")
set(TRANSLATIONS_URL "https://github.com/psi-plus/psi-plus-l10n.git")
set(PSIMEDIA_URL "${PSI_URL_PREFIX}/psimedia.git")
set(BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/build" CACHE STRING "Path to build dir")

find_package(Git REQUIRED)

get_filename_component(ABS_BUILD_DIR "${BUILD_DIR}" ABSOLUTE)
message(STATUS "Build dir: ${ABS_BUILD_DIR}")
set(PSI_SRC_DIR "${ABS_BUILD_DIR}/psi-build-src")

message(STATUS "CURRENT DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}")

if(EXISTS "${PSI_SRC_DIR}")
    message(STATUS "Removing directory: ${PSI_SRC_DIR}")
    file(REMOVE_RECURSE ${PSI_SRC_DIR})
    file(MAKE_DIRECTORY ${PSI_SRC_DIR})
endif()

if(NOT EXISTS "${ABS_BUILD_DIR}")
    message(STATUS "Creating directory: ${ABS_BUILD_DIR}")
    file(MAKE_DIRECTORY ${ABS_BUILD_DIR})
endif()

function(SubmodulesExists _PATH _OUTPUT)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} submodule status --recursive
        WORKING_DIRECTORY ${_PATH}
        OUTPUT_VARIABLE _HAS_SUBMODULES
    )
    if(NOT "${_HAS_SUBMODULES}" STREQUAL "")
        set(${_OUTPUT} ON PARENT_SCOPE)
    else()
        set(${_OUTPUT} OFF PARENT_SCOPE)
    endif()
endfunction()

function(FetchSRC _NAME _URL _OUTPATH)
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
            WORKING_DIRECTORY ${ABS_BUILD_DIR}
        )
        SubmodulesExists("${_OUTPATH}" HAS_SUBMODULES)
        if(HAS_SUBMODULES)
            message(STATUS "Cloning ${_NAME} git submodules...")
            execute_process(
                COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                WORKING_DIRECTORY ${_OUTPATH}
            )
        endif()
    endif()
endfunction()

function(Copy_SRC _INPUT_PATH _OUTPUT_PATH)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${_INPUT_PATH} ${_OUTPUT_PATH}
        WORKING_DIRECTORY ${ABS_BUILD_DIR}
    )
    message(STATUS "Copy ${_INPUT_PATH} to  ${_OUTPUT_PATH}")
endfunction()

function(Prepare_SRC _INPUT_PATH _SRC_PATH)
    message(STATUS "Copy ${_SRC_PATH} to  ${_INPUT_PATH}")
    execute_process(
        COMMAND ${GIT_EXECUTABLE} checkout-index -a -f --prefix=${_INPUT_PATH}/
        WORKING_DIRECTORY ${_SRC_PATH}
    )
    SubmodulesExists("${_SRC_PATH}" HAS_SUBMODULES)
    if(HAS_SUBMODULES)
        message(STATUS "Processing submodules...")
        execute_process(
            COMMAND ${GIT_EXECUTABLE} submodule foreach --recursive "${GIT_EXECUTABLE} checkout-index -a -f --prefix=${_INPUT_PATH}/$displaypath/"
            WORKING_DIRECTORY ${_SRC_PATH}
        )
    endif()
endfunction()

function(GetVersion _PATH _OUTPUT)
    execute_process (
        COMMAND ${GIT_EXECUTABLE} describe --tags
        WORKING_DIRECTORY ${_PATH}
        OUTPUT_VARIABLE PSI_TAG
    )
    string(REGEX MATCH "([0-9].[0-9])-.*" PSI_VER "${PSI_TAG}")
    if(CMAKE_MATCH_1)
        set(PSI_VER ${CMAKE_MATCH_1})
        execute_process(
            COMMAND ${GIT_EXECUTABLE} rev-list --count ${PSI_VER}..HEAD
            WORKING_DIRECTORY "${_PATH}"
            OUTPUT_VARIABLE PSI_REVISION
        )
    else()
        message(FATAL_ERROR "Can't obtain psi version")
    endif()
    execute_process( 
        COMMAND ${GIT_EXECUTABLE} log -n1 --date=short --pretty=format:'%ad'
        WORKING_DIRECTORY "${_PATH}"
        OUTPUT_VARIABLE PSI_DATE
    )
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
        WORKING_DIRECTORY ${_PATH}
        OUTPUT_VARIABLE HASH
    )
    set(VER_RESULT "${PSI_VER}.${PSI_REVISION} (${PSI_DATE}, ${HASH})")
    string(REGEX REPLACE "[']" "" VER_RESULT_TMP "${VER_RESULT}")
    string(REGEX REPLACE "\n" "" VER_RESULT_FINAL "${VER_RESULT_TMP}")
    set(${_OUTPUT} "${VER_RESULT_FINAL}" PARENT_SCOPE)
endfunction()

message(STATUS "Preparing psi sources...")
FetchSRC(Psi ${PSI_URL} "${ABS_BUILD_DIR}/psi")
message(STATUS "Preparing psi plugins sources...")
FetchSRC(Plugins ${PLUGINS_URL} "${ABS_BUILD_DIR}/plugins")
message(STATUS "Preparing psi translations sources...")
FetchSRC(Translations ${TRANSLATIONS_URL} "${ABS_BUILD_DIR}/translations")
message(STATUS "Preparing psimedia sources...")
FetchSRC(Psimeida ${PSIMEDIA_URL} "${ABS_BUILD_DIR}/psimedia")

GetVersion("${ABS_BUILD_DIR}/psi" PSI_VERSION)
message(STATUS "Psi version: ${PSI_VERSION}")
file(WRITE "${ABS_BUILD_DIR}/version.txt" ${PSI_VERSION})

message(STATUS "Preparing ${PSI_SRC_DIR}...")
if(EXISTS "${ABS_BUILD_DIR}/psi")
    Prepare_SRC(${PSI_SRC_DIR} "${ABS_BUILD_DIR}/psi")
    file(WRITE "${PSI_SRC_DIR}/version" ${PSI_VERSION})
endif()

if(EXISTS "${ABS_BUILD_DIR}/plugins")
    Prepare_SRC("${PSI_SRC_DIR}/plugins" "${ABS_BUILD_DIR}/plugins")
endif()

if(EXISTS "${ABS_BUILD_DIR}/translations")
    Prepare_SRC("${PSI_SRC_DIR}" "${ABS_BUILD_DIR}/translations")
endif()

if(EXISTS "${ABS_BUILD_DIR}/psimedia")
    Prepare_SRC("${PSI_SRC_DIR}/plugins/generic/psimedia" "${ABS_BUILD_DIR}/psimedia")
endif()
