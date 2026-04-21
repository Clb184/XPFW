# OpenGL test
add_executable(demo demo.cpp)

# Hot Reload for MSVC
if (MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	message("Enabling Hot Reload for MSVC")
    target_link_options(demo PRIVATE $<$<CONFIG:Debug>:/INCREMENTAL>)
    target_compile_options(demo PRIVATE $<$<CONFIG:Debug>:/ZI>)
endif()
target_include_directories(demo
	PUBLIC
	${XPFW_ROOT}/include/
	${XPFW_ROOT}/deps/glew/include/
)
target_link_libraries(demo PUBLIC ${GLLIB_NAME} DirectXMath)
set_target_properties(demo PROPERTIES DEBUGGER_WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}/demo/)

# Pack file test
add_executable(pack_demo pack_demo.cpp)

# Hot Reload for MSVC
if (MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	message("Enabling Hot Reload for MSVC")
    target_link_options(pack_demo PRIVATE $<$<CONFIG:Debug>:/INCREMENTAL>)
    target_compile_options(pack_demo PRIVATE $<$<CONFIG:Debug>:/ZI>)
endif()
target_include_directories(pack_demo
	PUBLIC
	${XPFW_ROOT}/include/
	${XPFW_ROOT}/deps/glew/include/
)
target_link_libraries(pack_demo PUBLIC ${GLLIB_NAME} DirectXMath)
set_target_properties(pack_demo PROPERTIES DEBUGGER_WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}/demo/)

# Pack file tool
add_executable(caf0_pack caf0_pack.cpp)

# Hot Reload for MSVC
if (MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	message("Enabling Hot Reload for MSVC")
    target_link_options(caf0_pack PRIVATE $<$<CONFIG:Debug>:/INCREMENTAL>)
    target_compile_options(caf0_pack PRIVATE $<$<CONFIG:Debug>:/ZI>)
endif()
target_include_directories(caf0_pack
	PUBLIC
	${XPFW_ROOT}/include/
	${XPFW_ROOT}/deps/glew/include/
)
target_link_libraries(caf0_pack PUBLIC ${GLLIB_NAME} DirectXMath)
set_target_properties(caf0_pack PROPERTIES DEBUGGER_WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}/demo/)

# Include path
message("Lib path: ${XPFW_ROOT}/include/")
