file(GLOB lib_hpp *.hpp)
file(GLOB lib_cpp *.cpp)


add_library(${GLLIB_NAME} ${lib_hpp} ${lib_cpp})
target_include_directories(${GLLIB_NAME} PUBLIC ${CMAKE_HOME_DIRECTORY}/include/)

message("Lib path: ${CMAKE_HOME_DIRECTORY}/include/")
# target_link_libraries(GLLIB_DEMO ${GLLIB_NAME})