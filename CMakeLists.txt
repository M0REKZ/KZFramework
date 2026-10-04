cmake_minimum_required(VERSION 3.20)
project(KZFramework)

set(CMAKE_CXX_STANDARD 11)

set(EXECUTABLE_NAME KZGameRunner)

if(${CMAKE_SYSTEM_NAME} MATCHES "Emscripten" AND NOT KZFRAMEWORK_GAME_DIR)
    message(FATAL_ERROR "KZFRAMEWORK_GAME_DIR is not set, current platform requires it")
endif()

# html
if(${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(KZFRAMEWORK_EMSCRIPTEN_C_FLAGS "-lidbfs.js -s ASYNCIFY -s ASYNCIFY_IMPORTS=[\"InitSavePathEmscripten\"] -s INITIAL_MEMORY=256MB")
    set(KZFRAMEWORK_EMSCRIPTEN_LINK_FLAGS "--use-preload-plugins --preload-file ${KZFRAMEWORK_GAME_DIR}@/game -sSTACK_SIZE=32MB")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${KZFRAMEWORK_EMSCRIPTEN_C_FLAGS} ${KZFRAMEWORK_EMSCRIPTEN_LINK_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KZFRAMEWORK_EMSCRIPTEN_C_FLAGS} ${KZFRAMEWORK_EMSCRIPTEN_LINK_FLAGS}")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

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
# emscripten does not build with this
set(SDLMIXER_MIDI_FLUIDSYNTH OFF CACHE BOOL "" FORCE)

add_subdirectory(${PROJECT_SOURCE_DIR}/lib/SDL3_mixer EXCLUDE_FROM_ALL)

# add SDL3_gfx:
### CMakeLists.txt included with this SDL3_gfx implementation requires a already installed SDL3
### But this project uses it as a submodule, causing it to throw a error while finding it
### So lets just build it as part of this project instead
set(SDL3_GFX_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL3_gfx)

file(GLOB SDL3_GFX_SOURCES CONFIGURE_DEPENDS
    ${SDL3_GFX_DIR}/*.c
    ${SDL3_GFX_DIR}/*.h
)

add_library(SDL3_GFX_LIB STATIC ${SDL3_GFX_SOURCES})

target_include_directories(SDL3_GFX_LIB
    PUBLIC
        ${SDL3_GFX_DIR}
)

### Tell SDL3_gfx where to find SDL3 headers
target_link_libraries(SDL3_GFX_LIB
    PUBLIC
        SDL3::SDL3
)

# add SDL3_ttf:
### need ttf libs
set(SDLTTF_VENDORED ON)
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/SDL3_ttf EXCLUDE_FROM_ALL)

if(${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    # add Lua5.1
    set(LUA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/Lua)

    file(GLOB LUA_SOURCES CONFIGURE_DEPENDS
        ${LUA_DIR}/*.c
        ${LUA_DIR}/*.h
    )

    add_library(liblua STATIC ${LUA_SOURCES})

    target_include_directories(liblua
        PUBLIC
            ${LUA_DIR}
    )
else()
    # add LuaJIT
    add_subdirectory(${PROJECT_SOURCE_DIR}/lib/LuaJIT EXCLUDE_FROM_ALL)
endif()

set(KZFRAMEWORK_SRC_FILES
    src/crossplatform.cpp
    src/crossplatform.h
    src/files.cpp
    src/files.h
    src/globals.cpp
    src/globals.h
    src/images.cpp
    src/images.h
    src/input.cpp
    src/input.h
    src/scripts.cpp
    src/scripts.h
    src/sounds.cpp
    src/sounds.h
    src/text.cpp
    src/text.h
    src/window.cpp
    src/window.h
    src/main.cpp
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
    add_library(${EXECUTABLE_NAME} SHARED ${KZFRAMEWORK_SRC_FILES})
else()
    add_executable(${EXECUTABLE_NAME} ${KZFRAMEWORK_SRC_FILES})
endif()

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

#html
if(CMAKE_SYSTEM_NAME MATCHES "Emscripten")
    target_link_options(${EXECUTABLE_NAME} PRIVATE
        ${KZFRAMEWORK_EMSCRIPTEN_LINK_FLAGS}
        -sNO_DISABLE_EXCEPTION_CATCHING
    )
    set(LUA_LIB liblua)
else()
    set(LUA_LIB libluajit)
endif()

target_link_libraries(${EXECUTABLE_NAME} PRIVATE
    SDL3::SDL3
    SDL3_mixer::SDL3_mixer
    SDL3_GFX_LIB
    SDL3_ttf::SDL3_ttf
    ${LUA_LIB}
)
