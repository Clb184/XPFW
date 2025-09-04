file(GLOB demo_hpp *.hpp)
file(GLOB demo_cpp *.cpp)

add_executable(demo ${demo_cpp})

# Hot Reload for MSVC
if (MSVC)
	message("Enabling Hot Reload for MSVC")
    target_link_options(demo PRIVATE $<$<CONFIG:Debug>:/INCREMENTAL>)
    target_compile_options(demo PRIVATE $<$<CONFIG:Debug>:/ZI>)
endif()
target_include_directories(demo
	PUBLIC
	${CMAKE_HOME_DIRECTORY}/include/
	${CMAKE_HOME_DIRECTORY}/deps/glew/include/
)
target_link_libraries(demo ${GLLIB_NAME} DirectXMath)
set_target_properties(demo PROPERTIES DEBUGGER_WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}/demo/)
message("Lib path: ${CMAKE_HOME_DIRECTORY}/include/")