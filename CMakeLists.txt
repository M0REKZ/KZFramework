cmake_minimum_required(VERSION 3.20)
project(KZFramework)

set(CMAKE_CXX_STANDARD 11)

set(EXECUTABLE_NAME KZGameRunner)

# set the output directory for built objects.
# this makes sure that the dynamic libraries goes into the build directory automatically.
set(KZFRAMEWORK_LIB_INSTALL_DIR ${CMAKE_BINARY_DIR}/local_libs)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${KZFRAMEWORK_LIB_INSTALL_DIR}")

# static only
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)

# dont build sdl3 tests
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)

# add SDL3 libs
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/SDL3 EXCLUDE_FROM_ALL)

set(SDLMIXER_VENDORED ON CACHE BOOL "" FORCE)

add_subdirectory(${PROJECT_SOURCE_DIR}/lib/SDL3_mixer EXCLUDE_FROM_ALL)


add_subdirectory(${PROJECT_SOURCE_DIR}/lib/LuaJIT EXCLUDE_FROM_ALL)

set(KZFRAMEWORK_SRC_FILES
    src/files.cpp
    src/files.h
    src/images.cpp
    src/images.h
    src/input.cpp
    src/input.h
    src/scripts.cpp
    src/scripts.h
    src/sounds.cpp
    src/sounds.h
    src/window.cpp
    src/window.h
    src/main.cpp
)

add_executable(${EXECUTABLE_NAME} ${KZFRAMEWORK_SRC_FILES})

target_include_directories(${EXECUTABLE_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# we should not need to include standard libraries as .dll/.so
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    target_link_options(${EXECUTABLE_NAME} PRIVATE -static-libgcc)
    target_link_options(${EXECUTABLE_NAME} PRIVATE -static-libstdc++)
elseif(MINGW)
    target_link_options(${EXECUTABLE_NAME} PRIVATE --static)
endif()

target_link_libraries(${EXECUTABLE_NAME} PRIVATE
    SDL3::SDL3
    SDL3_mixer::SDL3_mixer
    libluajit
)
