cmake_minimum_required(VERSION 3.13) # 2.2 - case insensitive syntax
                                     # 3.13 included policy CMP0077

project(pmbridge VERSION ${PROJECT_VERSION} LANGUAGES CXX)

set(PMBRIDGE_APP_NAME ${PROJECT_NAME})

message("PMBRIDGE: Start configure '${PMBRIDGE_APP_NAME}'")

configure_file(${CMAKE_CURRENT_LIST_DIR}/pmb_config.h.in ${CMAKE_CURRENT_LIST_DIR}/pmb_config.h)

set(MB_HEADERS
    pmb_config.h
    pmb_core.h
    pmb_log.h
    pmb_print.h
    pmbMemory.h
    pmbServer.h
    pmbClient.h
    pmbCommand.h
    pmbProject.h
    pmbBuilder.h
)

set(MB_SOURCES
    pmb_core.cpp
    pmb_log.cpp
    pmb_print.cpp
    pmbMemory.cpp
    pmbServer.cpp
    pmbClient.cpp
    pmbCommand.cpp
    pmbProject.cpp
    pmbBuilder.cpp
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

