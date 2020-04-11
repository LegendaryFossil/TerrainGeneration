cmake_minimum_required(VERSION 3.11)

if(POLICY CMP0025)
  cmake_policy(SET CMP0025 NEW)
endif()

project(
	"Terrain Generator"
	DESCRIPTION "Procedural Terrain Generator"
	LANGUAGES CXX
)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT TerrainGenerator)

if(WIN32)
	set(EXTERNAL_LIB_PATH "${PROJECT_SOURCE_DIR}/external_libs")

  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/build/Debug)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/build/Release)
  set(TERRAIN_GENERATOR_EXE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/build/$<$<CONFIG:DEBUG>:Debug>$<$<CONFIG:RELEASE>:Release>")
	set(CMAKE_INSTALL_PREFIX "")
    
	set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE STRING "" FORCE)
	set(CMAKE_SUPPRESS_REGENERATION true)
	set(CMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD 1)
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Always add external libs first 
add_subdirectory(external_libs)
add_subdirectory(src)
add_subdirectory(resources)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT TerrainGenerator)
else()
message(FATAL_ERROR "Unsupported platform")
endif()