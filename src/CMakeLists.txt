cmake_minimum_required(VERSION 3.13) # 2.2 - case insensitive syntax
                                     # 3.13 included policy CMP0077

project(pmbridge VERSION ${PROJECT_VERSION} LANGUAGES CXX)

set(PMB_APP_NAME ${PROJECT_NAME})

message(STATUS "PMBRIDGE: Start configure '${PMB_APP_NAME}'")

configure_file(${CMAKE_CURRENT_LIST_DIR}/core/pmb_config.h.in ${CMAKE_CURRENT_LIST_DIR}/core/pmb_config.h)

set(PMB_HEADERS
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

set(PMB_SOURCES
    core/pmb_core.cpp
    core/pmb_print.cpp
    core/pmb_help.cpp
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
    set(PMB_WIN_RESOURCE_FILE win_resource.rc)
        message(STATUS "PMBRIDGE: ${PROJECT_NAME} resource file for Windows: '${PMB_WIN_RESOURCE_FILE}'")

    set(PMB_WIN_FILE "${PROJECT_NAME}")
    set(PMB_WIN_OUTFILE "${PMB_APP_NAME}.exe")
    configure_file(${CMAKE_CURRENT_LIST_DIR}/${PMB_WIN_RESOURCE_FILE}.in 
                   ${CMAKE_CURRENT_LIST_DIR}/${PMB_WIN_RESOURCE_FILE})

    set(PMB_RESOURCES ${PMB_RESOURCES}
        ${CMAKE_CURRENT_LIST_DIR}/${PMB_WIN_RESOURCE_FILE}           
    )     
endif()

add_executable(${PMB_APP_NAME} ${PMB_HEADERS} ${PMB_SOURCES} ${PMB_RESOURCES})

include_directories(.
                    ..
                    ../modbus/src
                    core
                    log
)

set_target_properties(
    ${PMB_APP_NAME}
    PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    #WIN32_EXECUTABLE true
)

target_link_libraries(${PMB_APP_NAME} PRIVATE 
                      modbus
)

