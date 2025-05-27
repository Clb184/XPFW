file(GLOB lib_hpp *.hpp)
file(GLOB lib_cpp *.cpp)


add_library(${GLLIB_NAME} ${lib_hpp} ${lib_cpp})
target_link_libraries(${GLLIB_NAME} glfw glew_s)
target_include_directories(${GLLIB_NAME}
	PUBLIC
	${CMAKE_HOME_DIRECTORY}/include/
	${CMAKE_HOME_DIRECTORY}/deps/glew/include/
)

message("Lib path: ${CMAKE_HOME_DIRECTORY}/include/")