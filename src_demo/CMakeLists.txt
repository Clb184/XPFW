file(GLOB demo_hpp *.hpp)
file(GLOB demo_cpp *.cpp)

add_executable(GLLIB_DEMO ${demo_cpp})

# Hot Reload for MSVC
if (MSVC)
	message("Enabling Hot Reload for MSVC")
    target_link_options(GLLIB_DEMO PRIVATE $<$<CONFIG:Debug>:/INCREMENTAL>)
    target_compile_options(GLLIB_DEMO PRIVATE $<$<CONFIG:Debug>:/ZI>)
endif()
target_include_directories(GLLIB_DEMO
	PUBLIC
	${CMAKE_HOME_DIRECTORY}/include/
	${CMAKE_HOME_DIRECTORY}/deps/glew/include/
)
target_link_libraries(GLLIB_DEMO ${GLLIB_NAME} DirectXMath)
set_target_properties(GLLIB_DEMO PROPERTIES DEBUGGER_WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}/demo/)
message("Lib path: ${CMAKE_HOME_DIRECTORY}/include/")