cmake_minimum_required(VERSION 3.5)
project(GLScene VERSION 1.0.0)

if (UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")
endif (UNIX)

if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/libs/)
	execute_process(COMMAND mkdir ${CMAKE_CURRENT_BINARY_DIR}/libs/)
endif()

if (WIN32)
	if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/libs/glew)
		message(STATUS "Downloading glew...")
		file(DOWNLOAD https://github.com/nigels-com/glew/releases/download/glew-2.1.0/glew-2.1.0.zip ${CMAKE_CURRENT_BINARY_DIR}/libs/glew.zip)
		execute_process(COMMAND unzip -q ${CMAKE_CURRENT_BINARY_DIR}/libs/glew.zip -d ${CMAKE_CURRENT_BINARY_DIR}/libs/)
		execute_process(COMMAND mv ${CMAKE_CURRENT_BINARY_DIR}/libs/glew-2.1.0 ${CMAKE_CURRENT_BINARY_DIR}/libs/glew)
		execute_process(COMMAND rm ${CMAKE_CURRENT_BINARY_DIR}/libs/glew.zip)
	endif()
endif()

if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/libs/assimp)
	message(STATUS "Downloading assimp...")
	file(DOWNLOAD https://github.com/assimp/assimp/archive/v4.1.0.zip ${CMAKE_CURRENT_BINARY_DIR}/libs/assimp.zip)
	execute_process(COMMAND unzip -q ${CMAKE_CURRENT_BINARY_DIR}/libs/assimp.zip -d ${CMAKE_CURRENT_BINARY_DIR}/libs/)
	execute_process(COMMAND mv ${CMAKE_CURRENT_BINARY_DIR}/libs/assimp-4.1.0 ${CMAKE_CURRENT_BINARY_DIR}/libs/assimp)
	execute_process(COMMAND rm ${CMAKE_CURRENT_BINARY_DIR}/libs/assimp.zip)
endif()

if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/libs/glm)
	message(STATUS "Downloading glm...")
	file(DOWNLOAD https://github.com/g-truc/glm/releases/download/0.9.9.5/glm-0.9.9.5.zip ${CMAKE_CURRENT_BINARY_DIR}/libs/glm.zip)
	execute_process(COMMAND unzip -q ${CMAKE_CURRENT_BINARY_DIR}/libs/glm.zip -d ${CMAKE_CURRENT_BINARY_DIR}/libs/)
	execute_process(COMMAND mv ${CMAKE_CURRENT_BINARY_DIR}/libs/glm-0.9.9.5 ${CMAKE_CURRENT_BINARY_DIR}/libs/glm)
	execute_process(COMMAND rm ${CMAKE_CURRENT_BINARY_DIR}/libs/glm.zip)
endif()


file(GLOB_RECURSE SOURCE_FILES
libs/glew/src/glew.c
"sources/*.cpp"
"CLDevice/sources/*.cpp"

"includes/*.hpp"
"CLDevice/includes/*.hpp"
)

find_package(OpenCL REQUIRED)
find_package(OpenGL REQUIRED)

set(ASSIMP_BUILD_TESTS OFF CACHE BOOL " " FORCE)
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/libs/assimp)

include_directories(includes/)
include_directories(includes/GLScene/)
include_directories(CLDevice/includes/)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/build/libs/assimp/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/libs/assimp/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/libs/glew/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/libs/glm)

include_directories(${OpenCL_INCLUDE_DIRS})


add_library(GLScene STATIC ${SOURCE_FILES})
target_link_libraries(GLScene assimp ${OPENGL_LIBRARIES} ${OpenCL_LIBRARY})
