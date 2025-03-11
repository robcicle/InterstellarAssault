cmake_minimum_required(VERSION 3.28)
project(InterstellarAssault)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h" "data/*.lua")
file(GLOB_RECURSE HLSL_SOURCES "src/*.hlsl")

add_executable(InterstellarAssault ${SOURCES} ${HLSL_SOURCES})

foreach(HLSL_FILE ${HLSL_SOURCES})
    get_filename_component(FILE_NAME ${HLSL_FILE} NAME)
    set_source_files_properties(${HLSL_FILE} PROPERTIES
            VS_SHADER_MODEL 4.0
            VS_SHADER_OBJECT_FILE_NAME "$(OutDir)/data/shaders/%(Filename).cso"
        )
    if(FILE_NAME MATCHES "PS")
        set_source_files_properties(${HLSL_FILE} PROPERTIES
            VS_SHADER_TYPE Pixel
        )
    elseif(FILE_NAME MATCHES "VS")
        set_source_files_properties(${HLSL_FILE} PROPERTIES
            VS_SHADER_TYPE Vertex
        )
    endif()

    file(READ ${HLSL_FILE} HLSL_CONTENTS)
    if(HLSL_CONTENTS MATCHES "main")
        set_source_files_properties(${HLSL_FILE} PROPERTIES
            VS_SHADER_ENTRYPOINT main
        )
    else()
        set_source_files_properties(${HLSL_FILE} PROPERTIES
            HEADER_FILE_ONLY TRUE
        )
    endif()
endforeach()

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

include(FetchContent)

set(BUILD_SHARED_LIBS OFF)

# DirectXTK
FetchContent_Declare(
    DirectXTK
    GIT_REPOSITORY https://github.com/microsoft/DirectXTK.git
    GIT_TAG jul2022
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(DirectXTK)
target_link_libraries(InterstellarAssault PRIVATE DirectXTK)

# Assimp
FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG v5.2.5
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
set(ASSIMP_BUILD_STATIC_LIB ON CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(assimp)
target_link_libraries(InterstellarAssault PRIVATE assimp)

# Lua
FetchContent_Declare(
    lua
    GIT_REPOSITORY https://github.com/lua/lua.git
    GIT_TAG v5.4.4
)
FetchContent_MakeAvailable(lua)

# Manually add Lua sources
set_source_files_properties(${lua_SOURCE_DIR}/*.c PROPERTIES LANGUAGE CXX)
add_library(
    lua STATIC
    ${lua_SOURCE_DIR}/lapi.c
    ${lua_SOURCE_DIR}/lauxlib.c
    ${lua_SOURCE_DIR}/lbaselib.c
    ${lua_SOURCE_DIR}/lcode.c
    ${lua_SOURCE_DIR}/lcorolib.c
    ${lua_SOURCE_DIR}/lctype.c
    ${lua_SOURCE_DIR}/ldblib.c
    ${lua_SOURCE_DIR}/ldebug.c
    ${lua_SOURCE_DIR}/ldo.c
    ${lua_SOURCE_DIR}/ldump.c
    ${lua_SOURCE_DIR}/lfunc.c
    ${lua_SOURCE_DIR}/lgc.c
    ${lua_SOURCE_DIR}/linit.c
    ${lua_SOURCE_DIR}/liolib.c
    ${lua_SOURCE_DIR}/llex.c
    ${lua_SOURCE_DIR}/lmathlib.c
    ${lua_SOURCE_DIR}/lmem.c
    ${lua_SOURCE_DIR}/loadlib.c
    ${lua_SOURCE_DIR}/lobject.c
    ${lua_SOURCE_DIR}/lopcodes.c
    ${lua_SOURCE_DIR}/loslib.c
    ${lua_SOURCE_DIR}/lparser.c
    ${lua_SOURCE_DIR}/lstate.c
    ${lua_SOURCE_DIR}/lstring.c
    ${lua_SOURCE_DIR}/lstrlib.c
    ${lua_SOURCE_DIR}/ltable.c
    ${lua_SOURCE_DIR}/ltablib.c
    ${lua_SOURCE_DIR}/ltm.c
    ${lua_SOURCE_DIR}/lua.c
    ${lua_SOURCE_DIR}/lundump.c
    ${lua_SOURCE_DIR}/lutf8lib.c
    ${lua_SOURCE_DIR}/lvm.c
    ${lua_SOURCE_DIR}/lzio.c
)
target_include_directories(lua PUBLIC ${lua_SOURCE_DIR})
target_compile_definitions(lua PRIVATE LUA_USE_C89)

target_link_libraries(InterstellarAssault PRIVATE lua)
target_include_directories(InterstellarAssault PRIVATE ${lua_SOURCE_DIR})

target_link_libraries(InterstellarAssault PRIVATE XInput.lib)
target_link_libraries(InterstellarAssault PRIVATE d3d11 dxgi)

target_link_options(InterstellarAssault PRIVATE 
    "$<$<CONFIG:Debug>:/SUBSYSTEM:CONSOLE>"
    "$<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS>"
)

set_target_properties(InterstellarAssault PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>"
)

add_custom_command(TARGET InterstellarAssault POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/data $<TARGET_FILE_DIR:InterstellarAssault>/data)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT InterstellarAssault)