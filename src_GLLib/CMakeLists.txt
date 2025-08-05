file(GLOB lib_hpp *.hpp)
file(GLOB lib_cpp *.cpp)

# And create the library
add_library(${GLLIB_NAME} ${lib_hpp} ${lib_cpp})

# Hot Reload for MSVC
if (MSVC)
	message("Enabling Hot Reload for MSVC")
    target_link_options(${GLLIB_NAME} PRIVATE $<$<CONFIG:Debug>:/INCREMENTAL>)
    target_compile_options(${GLLIB_NAME} PRIVATE $<$<CONFIG:Debug>:/ZI>)
endif()

target_link_libraries(${GLLIB_NAME} glfw glew_s zlibstatic png_static miniaudio DirectXMath freetype)
target_include_directories(${GLLIB_NAME}
	PUBLIC
	${CMAKE_HOME_DIRECTORY}/include/
	${CMAKE_HOME_DIRECTORY}/deps/glew/include/
)

message("Lib path: ${CMAKE_HOME_DIRECTORY}/include/")