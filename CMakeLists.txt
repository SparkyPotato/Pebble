cmake_minimum_required(VERSION 3.16)

project(Pebble CXX)

find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
	option(SUBMODULE "Update submodules during build" ON)
	if(SUBMODULE)
		execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			RESULT_VARIABLE GIT_SUBMOD_RESULT)
		if(NOT GIT_SUBMOD_RESULT EQUAL "0")
			message(FATAL_ERROR "Updating submodules failed with code ${GIT_SUBMOD_RESULT}.")
		endif()
	endif()
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Libraries)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Libraries)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries)

set(THREADS_PREFER_PTHREAD_FLAG ON)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Don't build glfw examples")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Don't build glfw tests")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Don't build glfw docs")
add_subdirectory(External/glfw)
add_subdirectory(External/glm)
add_subdirectory(External/spdlog)
add_subdirectory(External/volk)
add_subdirectory(External/Vulkan-Headers)

set_target_properties(volk PROPERTIES COMPILE_DEFINITIONS VOLK_VULKAN_H_PATH="vulkan/vulkan.h")
target_link_libraries(volk PUBLIC Vulkan::Headers)

file(GLOB_RECURSE SOURCE CONFIGURE_DEPENDS 
	${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp
)
add_executable(Pebble ${SOURCE})

target_include_directories(Pebble PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source External/glm External/VMA/src)

target_precompile_headers(Pebble PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source/PCH.h)

target_compile_features(Pebble PRIVATE cxx_std_20)
set_target_properties(Pebble PROPERTIES CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)
target_link_libraries(Pebble PRIVATE Threads::Threads glfw glm spdlog volk)

file(GLOB_RECURSE GLSL_SOURCE CONFIGURE_DEPENDS
	${CMAKE_CURRENT_SOURCE_DIR}/Shaders/*
)

foreach(SHADER ${GLSL_SOURCE})
	get_filename_component(FILENAME ${SHADER} NAME)
	set(SPRIV "${PROJECT_BINARY_DIR}/Shaders/${FILENAME}.spv")
	add_custom_command(
		OUTPUT ${SPRIV}
		COMMAND glslc ${SHADER} -o ${SPRIV}
		DEPENDS ${SHADER}
	)
	list(APPEND SPIRV_FILES ${SPRIV})
endforeach()

add_custom_target(
	Shaders 
	DEPENDS ${SPIRV_FILES}
)
add_custom_command(
	TARGET Shaders
	COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/Shaders/"
)
add_dependencies(Pebble Shaders)
