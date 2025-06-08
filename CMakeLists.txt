cmake_minimum_required(VERSION 3.6)

project(GLLib2025 LANGUAGES C CXX) 

option(BUILD_DEMO ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)

file(DOWNLOAD https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.zip glew.zip)
file(DOWNLOAD https://raw.githubusercontent.com/dotnet/runtime/main/src/coreclr/pal/inc/rt/sal.h  ${CMAKE_HOME_DIRECTORY}/include/sal.h )

# Extract downloaded files
if(EXISTS ${CMAKE_HOME_DIRECTORY}/deps/glew)
	file(REMOVE_RECURSE ${CMAKE_HOME_DIRECTORY}/deps/glew)
endif()
file(ARCHIVE_EXTRACT INPUT glew.zip DESTINATION ${CMAKE_HOME_DIRECTORY}/deps PATTERNS glew-2.2.0/*)
file(RENAME ${CMAKE_HOME_DIRECTORY}/deps/glew-2.2.0 ${CMAKE_HOME_DIRECTORY}/deps/glew)

# Hot Reload for MSVC
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Dependencies
add_subdirectory(deps/glfw)

add_compile_definitions(GLEW_STATIC)
set(BUILD_UTILS OFF)
add_subdirectory(deps/glew/build/cmake)

# Build the library first (Also set name)
set(GLLIB_NAME GLLib2025)
add_subdirectory(src_GLLib)

# Maybe the demo?
add_subdirectory(src_demo)