cmake_minimum_required(VERSION 3.13) # 2.2 - case insensitive syntax
                                     # 3.13 included policy CMP0077

project(pmbridge VERSION ${PROJECT_VERSION} LANGUAGES CXX)

set(PMBRIDGE_APP_NAME ${PROJECT_NAME})

message("PMBRIDGE: Start configure '${PMBRIDGE_APP_NAME}'")

configure_file(${CMAKE_CURRENT_LIST_DIR}/core/pmb_config.h.in ${CMAKE_CURRENT_LIST_DIR}/core/pmb_config.h)

set(MB_HEADERS
    core/pmb_config.h
    core/pmb_core.h
    core/pmb_print.h
    log/pmbLogConsole.h
    log/pmb_log.h
    ${CMAKE_CURRENT_LIST_DIR}/project/pmbServer.h
    ${CMAKE_CURRENT_LIST_DIR}/project/pmbClient.h
    ${CMAKE_CURRENT_LIST_DIR}/project/pmbCommand.h
    ${CMAKE_CURRENT_LIST_DIR}/project/pmbProject.h
    ${CMAKE_CURRENT_LIST_DIR}/project/pmbBuilder.h
    pmbMemory.h
)

set(MB_SOURCES
    core/pmb_core.cpp
    core/pmb_print.cpp
    log/pmbLogConsole.cpp
    log/pmb_log.cpp
    ${CMAKE_CURRENT_LIST_DIR}/project/pmbServer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/project/pmbClient.cpp
    ${CMAKE_CURRENT_LIST_DIR}/project/pmbCommand.cpp
    ${CMAKE_CURRENT_LIST_DIR}/project/pmbProject.cpp
    ${CMAKE_CURRENT_LIST_DIR}/project/pmbBuilder.cpp
    pmbMemory.cpp
    pmbridge.cpp
)     

set(MB_RESOURCES 
)     

if (WIN32)
    set(PMBRIDGE_WIN_RESOURCE_FILE win_resource.rc)
        message("PMBRIDGE: ${PROJECT_NAME} resource file for Windows: '${PMBRIDGE_WIN_RESOURCE_FILE}'")

    set(PMBRIDGE_WIN_FILE "${PROJECT_NAME}")
    set(PMBRIDGE_WIN_OUTFILE "${PMBRIDGE_APP_NAME}.exe")
    configure_file(${CMAKE_CURRENT_LIST_DIR}/${PMBRIDGE_WIN_RESOURCE_FILE}.in 
                   ${CMAKE_CURRENT_LIST_DIR}/${PMBRIDGE_WIN_RESOURCE_FILE})

    set(MB_RESOURCES ${MB_RESOURCES}
        ${CMAKE_CURRENT_LIST_DIR}/${PMBRIDGE_WIN_RESOURCE_FILE}           
    )     
endif()

add_executable(${PMBRIDGE_APP_NAME} ${MB_HEADERS} ${MB_SOURCES} ${MB_RESOURCES})

include_directories(.
                    ..
                    ../modbus/src
                    core
                    log
)

set_target_properties(
    ${PMBRIDGE_APP_NAME}
    PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    #WIN32_EXECUTABLE true
)

target_link_libraries(${PMBRIDGE_APP_NAME} PRIVATE 
                      modbus
)

