set(OUTPUT_NAME test_structure_input)

file(GLOB_RECURSE SRC
    "*.cpp"
)

if (WINDOWS)
    if (RELEASE)
        # launch as windows, not console app - so cmd does not open as well
        add_link_options(-mwindows)
    endif ()
else()
    # so nautilus etc recognise target as executable rather than .so
    add_link_options(-no-pie)
endif()

include_directories(.)
include_directories(${CMAKE_SOURCE_DIR}/tests)

add_executable(${OUTPUT_NAME} ${SRC})

include(CTest)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/tests/cmake/)
message(${CMAKE_MODULE_PATH})
include(Catch)
set_target_properties(${OUTPUT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
catch_discover_tests(${OUTPUT_NAME})

file(COPY "CONFIG" DESTINATION "${CMAKE_BINARY_DIR}/tests/${OUTPUT_NAME}")
file(COPY "HISTORY" DESTINATION "${CMAKE_BINARY_DIR}/tests/${OUTPUT_NAME}")
file(COPY "psilocybin.xyz" DESTINATION "${CMAKE_BINARY_DIR}/tests/${OUTPUT_NAME}")
file(COPY "REVCON" DESTINATION "${CMAKE_BINARY_DIR}/tests/${OUTPUT_NAME}")