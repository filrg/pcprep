cmake_minimum_required(VERSION 3.21)

include(cmake/prelude.cmake)

project(
    pcprep
    VERSION 1.0.1
    DESCRIPTION "Point cloud preparation tool"
    HOMEPAGE_URL "https://quangnguyenlong.github.io/pcprep"
    LANGUAGES C CXX
)

include(cmake/variables.cmake)

# ---- Declare library ----

option(WITH_GL "Build with OPENGL" OFF)
option(WITH_GLFW "Build with GLFW" OFF)
option(WITH_GLEW "Build with GLEW" OFF)
option(DESKTOP_MODE "Build desktop mode" OFF)

# Collect source files
file(GLOB PCPREP_SHARED_SOURCE
    ${PROJECT_SOURCE_DIR}/source/*.c
    ${PROJECT_SOURCE_DIR}/source/*.cpp
    )

set_source_files_properties(source/cJSON.c PROPERTIES COMPILE_FLAGS "-Wno-error=float-equal")

if(DESKTOP_MODE)
    set(WITH_GL ON)
    set(WITH_GLFW ON)
    set(WITH_GLEW ON)
endif()

add_library(pcprep_pcprep ${PCPREP_SHARED_SOURCE})
add_library(pcprep::pcprep ALIAS pcprep_pcprep)

target_link_libraries(pcprep_pcprep png)

if(WITH_GL)
    target_link_libraries(pcprep_pcprep GL)
    target_compile_definitions(pcprep_pcprep PRIVATE WITH_GL)
    message(STATUS "GL support is enabled")
endif()
if(WITH_GLFW)
    target_link_libraries(pcprep_pcprep glfw)
    target_compile_definitions(pcprep_pcprep PRIVATE WITH_GLFW)
    message(STATUS "GLFW support is enabled")
endif()
if(WITH_GLEW)
    target_link_libraries(pcprep_pcprep GLEW)
    target_compile_definitions(pcprep_pcprep PRIVATE WITH_GLEW)
    message(STATUS "GLEW support is enabled")
endif()


include(GenerateExportHeader)
generate_export_header(
    pcprep_pcprep
    BASE_NAME pcprep
    EXPORT_FILE_NAME export/pcprep/pcprep_export.h
    )

if(NOT BUILD_SHARED_LIBS)
    target_compile_definitions(pcprep_pcprep PUBLIC PCPREP_STATIC_DEFINE)
endif()

set_target_properties(
    pcprep_pcprep PROPERTIES
    C_VISIBILITY_PRESET hidden
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    EXPORT_NAME pcprep
    OUTPUT_NAME pcprep
    )

target_include_directories(
    pcprep_pcprep ${warning_guard}
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
    )

# internal headers
target_include_directories(
    pcprep_pcprep ${warning_guard}
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/source/include>"
    )

target_include_directories(
    pcprep_pcprep SYSTEM
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/export>"
    )

target_compile_features(pcprep_pcprep PUBLIC c_std_99)

# ---- Application ----
if(PROJECT_IS_TOP_LEVEL)
    option(BUILD_APP "Build applications tree." "${pcprep_DEVELOPER_MODE}")
    if(BUILD_APP)
        add_subdirectory(pcp)
    endif()
endif()

if(PROJECT_IS_TOP_LEVEL)
    option(BUILD_VPCC "Build VPCC binaries for pcprep.sh." OFF)
    if(BUILD_VPCC)
        include(ExternalProject)
        ExternalProject_Add(
             vpcc
             GIT_REPOSITORY https://github.com/MPEGGroup/mpeg-pcc-tmc2.git
             GIT_TAG master
             CMAKE_ARGS
             -DCMAKE_INSTALL_PREFIX=${PROJECT_SOURCE_DIR}/external/vpcc
             )
    endif()
endif()

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
    include(cmake/install-rules.cmake)
endif()


# ---- Examples ----
if(PROJECT_IS_TOP_LEVEL)
    option(BUILD_EXAMPLES "Build examples tree." "${pcprep_DEVELOPER_MODE}")
    if(BUILD_EXAMPLES)
        add_subdirectory(example)
    endif()
endif()

# ---- Developer mode ----

if(NOT pcprep_DEVELOPER_MODE)
    return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
    message(
        AUTHOR_WARNING
        "Developer mode is intended for developers of pcprep"
        )
endif()

include(cmake/dev-mode.cmake)
