file(GLOB lib_hpp *.h)
file(GLOB lib_cpp *.c)

# And create the library
add_library(${GLLIB_NAME} STATIC ${lib_cpp})

# Hot Reload for MSVC
if (MSVC)
	message("Enabling Hot Reload for MSVC")
    target_link_options(${GLLIB_NAME} PRIVATE $<$<CONFIG:Debug>:/INCREMENTAL>)
    target_compile_options(${GLLIB_NAME} PRIVATE $<$<CONFIG:Debug>:/ZI>)
endif()
target_link_libraries(${GLLIB_NAME} PRIVATE glfw glew_s zlibstatic png_static miniaudio DirectXMath freetype vorbis vorbisfile ogg)
target_include_directories(${GLLIB_NAME}
	PUBLIC
	/include/
	/deps/glew/include/
)

message("Lib path: ${CMAKE_HOME_DIRECTORY}/include/")