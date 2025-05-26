cmake_minimum_required(VERSION 3.6)

project(GLLib2025 LANGUAGES C CXX) 

option(BUILD_DEMO ON)

# Hot Reload for MSVC
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(GLLIB_NAME GLLib2025)

# Build the library first
add_subdirectory(src_GLLib)

# Maybe the demo?
if(BUILD_DEMO)
	add_subdirectory(src_demo)
endif()