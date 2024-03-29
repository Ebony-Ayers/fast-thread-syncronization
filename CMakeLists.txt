cmake_minimum_required(VERSION 3.15)
if(TARGET fts)
    return()
endif()

# Set the project name to your project name, my project isn't very descriptive
project(fts CXX)
include(cmake/StandardProjectSettings.cmake)
include(cmake/PreventInSourceBuilds.cmake)

# Link this 'library' to set the c++ standard / compile-time options requested
if(NOT DEFINED fts_project_options)
	add_library(fts_project_options INTERFACE)
endif()
#target_compile_features(fts_project_options INTERFACE cxx_std_17)
include(cmake/CppStandard.cmake)

if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
  option(ENABLE_BUILD_WITH_TIME_TRACE "Enable -ftime-trace to generate time tracing .json files on clang" OFF)
  if(ENABLE_BUILD_WITH_TIME_TRACE)
    target_compile_options(fts_project_options INTERFACE -ftime-trace)
  endif()
endif()

# Link this 'library' to use the warnings specified in CompilerWarnings.cmake
if(NOT DEFINED fts_project_warnings)
	add_library(fts_project_warnings INTERFACE)
endif()

# enable cache system
include(cmake/Cache.cmake)

# standard compiler warnings
include(cmake/CompilerWarnings.cmake)
set_project_warnings(fts_project_warnings)

# sanitizer options if supported by compiler
include(cmake/Sanitizers.cmake)
enable_sanitizers(fts_project_options)

# allow for static analysis options
include(cmake/StaticAnalyzers.cmake)

option(BUILD_SHARED_LIBS "Enable compilation of shared libraries" OFF)

# Very basic PCH example
option(ENABLE_PCH "Enable Precompiled Headers" ON)
if(ENABLE_PCH)
  # This sets a global PCH parameter, each project will build its own PCH, which is a good idea if any #define's change
  #
  # consider breaking this out per project as necessary
  target_precompile_headers(
    fts_project_options
    INTERFACE
    <atomic>)
endif()

find_package(Threads)

add_subdirectory(src)

target_link_libraries(fts PUBLIC ${CMAKE_THREAD_LIBS_INIT})
if(MSVC)
    target_link_libraries(fts PUBLIC Synchronization)
endif()
option(ENABLE_UNITY "Enable Unity builds of projects" OFF)
if(ENABLE_UNITY)
  # Add for any project you want to apply unity builds for
  set_target_properties(fts PROPERTIES UNITY_BUILD ON)
endif()
