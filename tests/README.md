# pmbridge Test Suite

This document explains how the pmbridge test framework is organized and how to build,
run, and extend tests on Windows using CMake and GoogleTest.

## Overview

- Framework: CMake + GoogleTest (via `gtest_dependency.cmake`).
- Test target: a single executable `testpmbridge` that includes project sources and all test files.
- Discovery: Tests are registered with CTest using `gtest_discover_tests` at configure/generate time; they are run via `ctest`.
- Layout: Tests are grouped by domain under `tests/core`, `tests/log`, `tests/project`, plus `tests/pmbMemory_test.cpp` and `tests/main.cpp`.

## Folder Structure

- `tests/CMakeLists.txt`: Builds test executable, links modbus, and registers tests with CTest.
- `tests/gtest_dependency.cmake`: Fetches or locates GoogleTest sources.
- `tests/main.cpp`: GoogleTest entry point.
- `tests/core/*.cpp`: Core utilities and formatting tests.
- `tests/log/*.cpp`: Logging and console formatting tests.
- `tests/project/*.cpp`: Builder, client, server, command, and project tests.
- `tests/pmbMemory_test.cpp`: Memory and Modbus address operations tests.

## Prerequisites

- C++17 compatible compiler.
- CMake 3.20+.
- Installed GoogleTest sources in `external/googletest`.

## Configure and Build

You can build the tests either via CMake Presets (recommended) or manually. Replace paths as needed.

### Using CMake Presets

If a `CMakePresets.json` includes a tests preset (e.g., `Win64-Tests-Debug`), run:

```powershell
cmake --preset Win64-Tests-Debug
cmake --build --preset Win64-Tests-Debug
```

### Manual Configure/Build

From the repo root (`c:\Users\march\Dropbox\PRJ\pmbridge`):

```powershell
# Create a build directory for tests
$buildDir = "build-tests"; New-Item -ItemType Directory -Force -Path $buildDir | Out-Null

# Configure with CTest enabled
cmake -S . -B $buildDir -D CMAKE_BUILD_TYPE=Debug -D BUILD_TESTING=ON

# Build the test executable
cmake --build $buildDir --config Debug
```

## Run Tests

Tests are executed with `ctest`. Use verbose output to see failing details.

```powershell
# From the build dir used above
ctest --test-dir $buildDir -C Debug --output-on-failure
```

If using presets, CTest typically runs in the preset's build directory:

```powershell
ctest --preset Win64-Tests-Debug --output-on-failure
```

### Run the GoogleTest executable directly

You can also run the compiled test runner `testpmbridge` without `ctest`. This is useful for quick iterations or filtering with GoogleTest flags.

```powershell
# From the build dir containing testpmbridge
./testpmbridge.exe --gtest_color=yes --gtest_output=xml:reports/gtest.xml

# Filter to a specific suite/case using GoogleTest
./testpmbridge.exe --gtest_filter=Log.*
```

Notes:

- The entry point is `tests/main.cpp`, which calls `RUN_ALL_TESTS()`.
- `ctest` adds integration (reporting, presets, working directories),
but direct execution is equivalent for running tests.

## VS Code Integration

- The workspace includes CMake and GoogleTest integration;
VS Code Test Explorer can discover tests if the CMake Tools extension is installed.
- A tasks entry for building a single C++ file exists, but for tests use the CMake build commands above.

## Adding New Tests

1. Place your test in the appropriate folder:
   - Core: `tests/core/your_feature_test.cpp`
   - Log: `tests/log/your_logging_test.cpp`
   - Project: `tests/project/your_component_test.cpp`
   - Memory: add to `tests/pmbMemory_test.cpp` or create a new file at `tests/your_memory_test.cpp`.
2. Register the file in `tests/CMakeLists.txt` under `PMB_TESTS_SOURCES`.
3. Include needed headers; tests already include project sources so most symbols are available.
4. Use GoogleTest macros:
   - `TEST(SuiteName, CaseName) { /* ... */ }`
   - `EXPECT_EQ`, `ASSERT_TRUE`, etc.
5. Build and run via the commands above. CTest will auto-discover tests from the executable.

## Conventions Used in Existing Tests

- Keep assertions resilient to implementation specifics (e.g., byte/bit packing for coils).
- Prefer validating round-trip behavior (write → read → compare) over internal representation checks.
- Use project APIs and helpers (e.g., `pmb::toFormat`, `pmbMemory::Block`, `Modbus::Address`)
rather than mocking internals.
- Logging console tests focus on token parsing semantics (`Time`, `Category`, `Literal`, `Text`)
and supported time formats.
- Memory tests favor register reads/writes where behavior is stable across platforms.

## Troubleshooting

- Missing GoogleTest: download GoogleTest sources into `external/googletest`.
- Build errors in tests: Confirm `BUILD_TESTING=ON` and that `PMB_TESTS_SOURCES` includes your test file.
- Linker errors: The tests link against `modbus`; ensure the modbus target builds in your configuration.
- Test discovery issues: Delete the build directory and reconfigure;
- CTest discovery happens at build/generate time.

## Useful CTest Flags

- `-R <regex>`: Run only tests matching a name pattern.
- `-E <regex>`: Exclude tests by pattern.
- `-j <N>`: Parallel test jobs.
- `--repeat until-pass:3`: Retry flaky tests up to 3 times.

## Example: Run Only Log Tests

```powershell
ctest --test-dir $buildDir -C Debug -R "log"
```

## Notes

- The tests executable includes many project sources directly (see `PMB_SRC_SOURCES`).
This ensures tests compile against current code without separate library targets.
- `WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests` is set for test runs;
files written/read during tests will use that working directory.
