cmake_minimum_required(VERSION 3.6)

project(GLLib2025 LANGUAGES C CXX) 

option(BUILD_DEMO ON)

set(GLLIB_NAME GLLib2025)

if(BUILD_DEMO)
	add_subdirectory(src_demo)
endif()