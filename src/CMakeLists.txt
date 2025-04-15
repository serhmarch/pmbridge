cmake_minimum_required(VERSION 3.13) # 2.2 - case insensitive syntax
                                     # 3.13 included policy CMP0077

project(mpbridge VERSION ${PROJECT_VERSION} LANGUAGES CXX)

set(MBRIDGE_APP_NAME ${PROJECT_NAME})

message("MBRIDGE: Start configure '${MBRIDGE_APP_NAME}'")

configure_file(${CMAKE_CURRENT_LIST_DIR}/mb_config.h.in ${CMAKE_CURRENT_LIST_DIR}/mb_config.h)

set(MB_HEADERS
    mb_config.h
    mb_core.h
    mbMemory.h
    mbServer.h
    mbClient.h
    mbCommand.h
    mbProject.h
    mbBuilder.h
)

set(MB_SOURCES
    mb_core.cpp
    mbMemory.cpp
    mbServer.cpp
    mbClient.cpp
    mbCommand.cpp
    mbProject.cpp
    mbBuilder.cpp
    mbridge.cpp
)     

set(MB_RESOURCES 
)     

if (WIN32)
    set(MBRIDGE_WIN_RESOURCE_FILE win_resource.rc)
        message("${PROJECT_NAME} resource file for Windows: '${MBRIDGE_WIN_RESOURCE_FILE}'")

    set(MBRIDGE_WIN_FILE "${PROJECT_NAME}")
    set(MBRIDGE_WIN_OUTFILE "${MBRIDGE_APP_NAME}.exe")
    configure_file(${CMAKE_CURRENT_LIST_DIR}/${MBRIDGE_WIN_RESOURCE_FILE}.in 
                   ${CMAKE_CURRENT_LIST_DIR}/${MBRIDGE_WIN_RESOURCE_FILE})

    set(MB_RESOURCES ${MB_RESOURCES}
        ${CMAKE_CURRENT_LIST_DIR}/${MBRIDGE_WIN_RESOURCE_FILE}           
    )     
endif()

add_executable(${MBRIDGE_APP_NAME} ${MB_HEADERS} ${MB_SOURCES} ${MB_RESOURCES})

include_directories(.
                    ..
                    ../modbus/src
)

set_target_properties(
    ${MBRIDGE_APP_NAME}
    PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    #WIN32_EXECUTABLE true
)

target_link_libraries(${MBRIDGE_APP_NAME} PRIVATE 
                      modbus
)

