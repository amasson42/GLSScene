cmake_minimum_required(VERSION 3.5)
project(GLScene VERSION 1.0.0)

if (UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O2 -pthread")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest /O2")
endif()

if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/libs/)
	execute_process(COMMAND mkdir ${CMAKE_CURRENT_BINARY_DIR}/libs/)
endif()

if (WIN32)
	if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/libs/glad)
		message(STATUS "Downloading glad...")
		file(DOWNLOAD https://github.com/Dav1dde/glad/archive/v0.1.33.zip ${CMAKE_CURRENT_BINARY_DIR}/libs/glad.zip)
		execute_process(COMMAND unzip -q ${CMAKE_CURRENT_BINARY_DIR}/libs/glad.zip -d ${CMAKE_CURRENT_BINARY_DIR}/libs/)
		execute_process(COMMAND python -m glad --profile=core --api=gl=4.6 --generator=c --spec=gl --no-loader --extensions= --out-path=${CMAKE_CURRENT_BINARY_DIR}/libs/glad WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libs/glad-0.1.33)
		execute_process(COMMAND rm ${CMAKE_CURRENT_BINARY_DIR}/libs/glad.zip)
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
include_directories(${CMAKE_CURRENT_BINARY_DIR}/libs/glad/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/libs/glm)

include_directories(${OpenCL_INCLUDE_DIRS})


add_library(GLScene STATIC ${SOURCE_FILES})
target_link_libraries(GLScene assimp ${OPENGL_LIBRARIES} ${OpenCL_LIBRARY})
