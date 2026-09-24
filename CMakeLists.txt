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
### we will put SDL3::Headers in SDL3_INCLUDE_DIR to pass it into SDL3_gfx
get_target_property(SDL3_INCLUDE_DIR SDL3::Headers INTERFACE_INCLUDE_DIRECTORIES)

add_subdirectory(${PROJECT_SOURCE_DIR}/lib/SDL3_gfx EXCLUDE_FROM_ALL)

### dont build shared
if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set_target_properties(SDL3_gfx_Shared PROPERTIES EXCLUDE_FROM_ALL TRUE)
endif()
set(SDL3_GFX_LIB SDL3_gfx_Static)

# add SDL3_ttf:
### need ttf libs
set(SDLTTF_VENDORED ON)
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/SDL3_ttf EXCLUDE_FROM_ALL)

if(${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    # add Lua5.1
    set(LUA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/Lua)

    set(LUA_SOURCES
        ${LUA_DIR}/lapi.c
        ${LUA_DIR}/lauxlib.c
        ${LUA_DIR}/lbaselib.c
        ${LUA_DIR}/lcode.c
        ${LUA_DIR}/ldblib.c
        ${LUA_DIR}/ldebug.c
        ${LUA_DIR}/ldo.c
        ${LUA_DIR}/ldump.c
        ${LUA_DIR}/lfunc.c
        ${LUA_DIR}/lgc.c
        ${LUA_DIR}/linit.c
        ${LUA_DIR}/liolib.c
        ${LUA_DIR}/llex.c
        ${LUA_DIR}/lmathlib.c
        ${LUA_DIR}/lmem.c
        ${LUA_DIR}/loadlib.c
        ${LUA_DIR}/lobject.c
        ${LUA_DIR}/lopcodes.c
        ${LUA_DIR}/loslib.c
        ${LUA_DIR}/lparser.c
        ${LUA_DIR}/lstate.c
        ${LUA_DIR}/lstring.c
        ${LUA_DIR}/lstrlib.c
        ${LUA_DIR}/ltable.c
        ${LUA_DIR}/ltablib.c
        ${LUA_DIR}/ltm.c
        ${LUA_DIR}/lundump.c
        ${LUA_DIR}/lvm.c
        ${LUA_DIR}/lzio.c
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

add_executable(${EXECUTABLE_NAME} ${KZFRAMEWORK_SRC_FILES})

target_include_directories(${EXECUTABLE_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    SDL3_gfx::Headers
)

# we should not need to include standard libraries as .dll/.so
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    target_link_options(${EXECUTABLE_NAME} PRIVATE -static-libgcc)
    target_link_options(${EXECUTABLE_NAME} PRIVATE -static-libstdc++)
elseif(MINGW)
    target_link_options(${EXECUTABLE_NAME} PRIVATE --static)
endif()

#html
if( ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    target_link_options(${EXECUTABLE_NAME} PRIVATE
        ${KZFRAMEWORK_EMSCRIPTEN_LINK_FLAGS}
    )

    target_link_libraries(${EXECUTABLE_NAME} PRIVATE
        SDL3::SDL3
        SDL3_mixer::SDL3_mixer
        ${SDL3_GFX_LIB}
        SDL3_ttf::SDL3_ttf
        liblua
    )
else()
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE
        SDL3::SDL3
        SDL3_mixer::SDL3_mixer
        ${SDL3_GFX_LIB}
        SDL3_ttf::SDL3_ttf
        libluajit
    )
endif()
