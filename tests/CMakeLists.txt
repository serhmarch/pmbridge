message(STATUS "PMBRIDGE: Tests included")

include(${CMAKE_CURRENT_SOURCE_DIR}/gtest_dependency.cmake)

set(PMB_SRC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/pmb_config.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/pmb_core.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/pmb_print.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/log/pmbLogConsole.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/log/pmb_log.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/project/pmbServer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/project/pmbClient.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/project/pmbCommand.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/project/pmbProject.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/project/pmbBuilder.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/pmbMemory.h
)

set(PMB_SRC_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/pmb_core.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/pmb_print.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/pmb_help.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/log/pmbLogConsole.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/log/pmb_log.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/project/pmbServer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/project/pmbClient.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/project/pmbCommand.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/project/pmbProject.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/project/pmbBuilder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/pmbMemory.cpp
)     

set(PMB_TESTS_HEADERS 
#    MockModbusPort.h
#    MockModbusDevice.h
    )

set(PMB_TESTS_SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/project/pmbBuilder_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/project/pmbClient_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/project/pmbServer_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/project/pmbCommand_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/project/pmbProject_test.cpp
    main.cpp
    )

set(PMB_TESTS_EXEC_NAME testpmbridge)

include_directories(.
                    ..
                    ../modbus/src
                    ../src
                    ../src/core
                    ../src/log
)

add_executable(${PMB_TESTS_EXEC_NAME}
               ${GOOGLETEST_SOURCES}
               ${PMB_SRC_HEADERS}
               ${PMB_SRC_SOURCES}
               ${PMB_TESTS_HEADERS}
               ${PMB_TESTS_SOURCES}
)
target_link_libraries(${PMB_TESTS_EXEC_NAME} PRIVATE modbus)

# --- CTest integration ---
# Register tests with CTest via GoogleTest discovery
if (BUILD_TESTING)
    include(GoogleTest)
    # Avoid running the test executable at build time; discover at test time
    gtest_discover_tests(${PMB_TESTS_EXEC_NAME}
        DISCOVERY_MODE PRE_TEST
        # Ensure tests run with working directory in the tests folder
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests
    )
    # Also add a top-level test to run the whole suite if desired
    add_test(NAME run-all-tests COMMAND ${PMB_TESTS_EXEC_NAME})
endif()
