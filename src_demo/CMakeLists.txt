file(GLOB demo_hpp *.hpp)
file(GLOB demo_cpp *.cpp)

add_executable(GLLIB_DEMO ${demo_hpp} ${demo_cpp})
target_link_directories(GLLIB_DEMO ${CMAKE_HOME_DIRECTORY}/include/}
message("Lib path: ${CMAKE_HOME_DIRECTORY}/include/")
# target_link_libraries(GLLIB_DEMO ${GLLIB_NAME})