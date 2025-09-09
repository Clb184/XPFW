cmake_minimum_required(VERSION 3.5)

project(XPFW LANGUAGES C CXX) 

option(BUILD_DEMO ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)

# If you don't have a copy of GLEW and are just looking over here, have this link, this is what I use:
# https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.zip

message("Directory is ${CMAKE_CURRENT_SOURCE_DIR}")
message("Make sure to have a copy of GLEW in the 'deps' folder, sorry that I can't just add it as a submodule, I do this because the repository is incomplete and to not be dependent on Internet connection")

# sal.h for DirectXMath
if(MSVC)
	message("sal.h already comes with VS...")
else()
	message("Getting sal.h for DirectXMath")
	file(DOWNLOAD https://raw.githubusercontent.com/dotnet/runtime/main/src/coreclr/pal/inc/rt/sal.h  ${CMAKE_HOME_DIRECTORY}/include/sal.h )
endif()

if("" STREQUAL "${CMAKE_BUILD_TYPE}")
	message("Configuration not selected, defaulting to debug")
	set(CMAKE_BUILD_TYPE "Debug")
	message("Config is now ${CMAKE_BUILD_TYPE}")
endif()

# Set some variables to disable stuff I don't want to build.
# I only use static libs so I have less files to deal with,
# and also use some debug postfix like d for debug builds
set(CMAKE_DEBUG_POSTFIX "d")

# Replace the "Release" string however was written so
# the CMAKE_DEBUG_POSTFIX variable can be "removed"
string(REGEX REPLACE "([Rr][Ee][Ll][Ee][Aa][Ss][Ee])" "RELEASE" RELEASE_CFG ${CMAKE_BUILD_TYPE})

if("RELEASE" STREQUAL ${RELEASE_CFG})
set(CMAKE_DEBUG_POSTFIX "")
endif()

message("Debug postfix is: ${CMAKE_DEBUG_POSTFIX} for ${CMAKE_BUILD_TYPE}" )

set(ZLIB_BUILD_STATIC ON)
set(ZLIB_BUILD_SHARED OFF)
set(PNG_STATIC ON)
set(PNG_SHARED OFF)
set(PNG_TESTS OFF)
set(PNG_TOOLS OFF)
set(PNG_EXECUTABLES OFF)
if(WIN32)
set(ZLIB_LIBRARY "${CMAKE_CURRENT_BINARY_DIR}/deps/zlib/zs${CMAKE_DEBUG_POSTFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}" )
message("Binary dir for ZLib should be ${CMAKE_CURRENT_BINARY_DIR}/deps/zlib/")
endif()
set(ZLIB_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/zlib/" )
set(ZLIB_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/deps/zlib/" )
set(FT_DISABLE_HARFBUZZ ON)
set(FT_REQUIRE_HARFBUZZ OFF)
set(FT_DYNAMIC_HARFBUZZ OFF)

# Add necessary GLFW, ZLib and LibPNG
add_subdirectory(deps/glfw)
add_subdirectory(deps/zlib/)
add_subdirectory(deps/libpng/)
add_subdirectory(deps/ogg)
add_subdirectory(deps/vorbis)
add_subdirectory(deps/miniaudio)
add_subdirectory(deps/FreeType)
add_subdirectory(deps/DirectXMath)

add_compile_definitions(GLEW_STATIC)
set(BUILD_UTILS OFF)
add_subdirectory(deps/glew/build/cmake)

# Build the library first (Also set name)
set(GLLIB_NAME XPFW)
set(XPFW_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(src)

# Maybe the demo?
if(${BUILD_DEMO})
add_subdirectory(src_demo)
endif()