cmake_minimum_required(VERSION 3.1)
project(sfoav)

list(APPEND CMAKE_CXX_SOURCE_FILE_EXTENSIONS shader)

set(CMAKE_CXX_STANDARD 17)
if (RELEASE)
    if (ANDROID)
        message("ANDROID MinSizeRel!")
        # GOOGLE!! (Release is not a thing pos ndk 23 apparently..)
        add_compile_definitions(BUILD_TYPE="MinSizeRel")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-trapping-math -fno-rounding-math -fno-signed-zeros")
    else()
        set(CMAKE_BUILD_TYPE Release)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-trapping-math -fno-rounding-math -fno-signaling-nans -fno-signed-zeros")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -O3 -Wall")
    endif()
else()
    if (ANDROID)
        # GOOGLE!!
        add_compile_definitions(BUILD_TYPE="RelWithDebInfo")
    else()
        set(CMAKE_BUILD_TYPE Debug)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -g -Wall")
    endif()
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

set(OUTPUT_NAME sfoav)

include_directories(include)

if (WINDOWS)
    set(LIB_JGL "${PROJECT_SOURCE_DIR}/include/libjGL-windows.a")
    add_compile_definitions(WINDOWS)
    if (RELEASE)
        # launch as windows, not console app - so cmd does not open as well
        add_link_options(-mwindows)
    endif ()
elseif(OSX)
    set(LIB_JGL "${PROJECT_SOURCE_DIR}/include/libjGL-macos.a")
    add_compile_definitions(APPLE)
else ()
    add_link_options(-no-pie)
    set(LIB_JGL "${PROJECT_SOURCE_DIR}/include/libjGL-linux-x86_64.a")
endif()

if (PROFILE)
    # apt-get install google-perftools libgoogle-perftools-dev google-pprof
    # CPUPROFILE=prof.out ./OUTPUT_NAME
    # google-pprof --pdf OUTPUT_NAME prof.out > sbt.pdf
    add_link_options("-Wl,--no-as-needed,-lprofiler,--as-needed")
endif()

add_executable(${OUTPUT_NAME}
    "src/main.cpp"
)

target_compile_definitions(${OUTPUT_NAME} PUBLIC GLSL_VERSION="330")
target_compile_definitions(${OUTPUT_NAME} PUBLIC MAX_SPRITE_BATCH_BOUND_TEXTURES=4)

find_package(ZLIB REQUIRED)
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

set_target_properties(${OUTPUT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${OUTPUT_NAME}")

file(GLOB RES "${PROJECT_SOURCE_DIR}/common/res/*")
file(COPY ${RES} DESTINATION "${CMAKE_BINARY_DIR}/${OUTPUT_NAME}/res")

if (WINDOWS)
    file(GLOB DLL "${PROJECT_SOURCE_DIR}/common/windows/*.dll")
    file(COPY ${DLL} DESTINATION "${CMAKE_BINARY_DIR}/${OUTPUT_NAME}/")
endif()