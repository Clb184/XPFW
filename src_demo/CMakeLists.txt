file(GLOB demo_hpp *.hpp)
file(GLOB demo_cpp *.cpp)

add_executable(GLLIB_DEMO ${demo_hpp} ${demo_cpp})
target_include_directories(GLLIB_DEMO PUBLIC ${CMAKE_HOME_DIRECTORY}/include/}
target_link_libraries(GLLIB_DEMO ${GLLIB_NAME})
message("Lib path: ${CMAKE_HOME_DIRECTORY}/include/")
# target_link_libraries(GLLIB_DEMO ${GLLIB_NAME})