enable_testing()
add_subdirectory(models)

set(OUTPUT_NAME unit_tests)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/tests/cmake/)

if (WINDOWS)
    if (RELEASE)
        # launch as windows, not console app - so cmd does not open as well
        add_link_options(-mwindows)
    endif ()
else()
    # so nautilus etc recognise target as executable rather than .so
    add_link_options(-no-pie)
endif()

include_directories(${CMAKE_SOURCE_DIR}/tests)

add_executable(${OUTPUT_NAME} "unit_tests.cpp")

include(CTest)
include(Catch)
set_target_properties(${OUTPUT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
catch_discover_tests(${OUTPUT_NAME} WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

if (NOT WINDOWS AND NOT OSX)
    if(CMAKE_COMPILER_IS_GNUCXX)
        include(CodeCoverage)
        append_coverage_compiler_flags_to_target(${OUTPUT_NAME})
        setup_target_for_coverage_gcovr(
            NAME ctest_coverage
            EXECUTABLE ctest
            DEPENDENCIES ${OUTPUT_NAME}
            EXCLUDE "include/vendored/*" "tests/*"
        )
    endif()
endif()

file(COPY "test_structure_input/CONFIG" DESTINATION "${CMAKE_BINARY_DIR}")
file(COPY "test_structure_input/HISTORY" DESTINATION "${CMAKE_BINARY_DIR}")
file(COPY "test_structure_input/psilocybin.xyz" DESTINATION "${CMAKE_BINARY_DIR}")
file(COPY "test_structure_input/REVCON" DESTINATION "${CMAKE_BINARY_DIR}")
file(COPY "test_structure_input/ethanol.REVCON" DESTINATION "${CMAKE_BINARY_DIR}")

file(COPY "test_elements/CPK" DESTINATION "${CMAKE_BINARY_DIR}")