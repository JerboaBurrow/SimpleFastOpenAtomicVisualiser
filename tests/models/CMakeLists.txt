set(OUTPUT_NAME sfoav_models_test)

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

add_executable(${OUTPUT_NAME} ${SRC})

target_compile_definitions(${OUTPUT_NAME} PUBLIC GLSL_VERSION="330")
target_compile_definitions(${OUTPUT_NAME} PUBLIC MAX_SPRITE_BATCH_BOUND_TEXTURES=4)

if(NOT WINDOWS)
    # ubuntu has a libz-mingw-w64-dev but not a libpng-mingw-w64-dev...
    # there are no link errors...
    find_package(PNG REQUIRED)
endif()
find_package(Vulkan REQUIRED)
find_package(OpenGL REQUIRED)
find_package(X11 REQUIRED)

target_link_libraries(${OUTPUT_NAME}
    ${LIB_JGL}
    ${X11_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${Vulkan_LIBRARIES}
    ${ZLIB_LIBRARIES}
    ${CMAKE_DL_LIBS}
)

if(NOT WINDOWS)
    target_link_libraries(${OUTPUT_NAME}
        ${PNG_LIBRARIES}
    )
endif()

target_include_directories(
	${OUTPUT_NAME}
	PUBLIC
	${Vulkan_INCLUDE_DIR}
)

if (OSX)
    # https://stackoverflow.com/questions/18391487/compiling-with-glfw3-linker-errors-undefined-reference
    target_link_libraries(${OUTPUT_NAME} "-framework Cocoa -framework IOKit -framework CoreVideo")
endif ()

if (WINDOWS)
    target_link_libraries(${OUTPUT_NAME} "winmm")
endif ()
set_target_properties(${OUTPUT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")