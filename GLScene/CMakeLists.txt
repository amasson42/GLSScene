cmake_minimum_required(VERSION 3.5)
project(GLScene VERSION 1.0.0)

if (UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")
endif (UNIX)

if (NOT EXISTS ../libs/)
	execute_process(COMMAND mkdir ../libs/)
endif()

if (WIN32)
	if (NOT EXISTS ../libs/glew)
		message(STATUS "Downloading glew...")
		file(DOWNLOAD https://github.com/nigels-com/glew/releases/download/glew-2.1.0/glew-2.1.0.zip ../libs/glew.zip)
		execute_process(COMMAND unzip -q ../libs/glew.zip -d ../libs/)
		execute_process(COMMAND mv ../libs/glew-2.1.0 ../libs/glew)
		execute_process(COMMAND rm ../libs/glew.zip)
	endif()
endif()

if (NOT EXISTS ../libs/assimp)
	message(STATUS "Downloading assimp...")
	file(DOWNLOAD https://github.com/assimp/assimp/archive/v4.1.0.zip ../libs/assimp.zip)
	execute_process(COMMAND unzip -q ../libs/assimp.zip -d ../libs/)
	execute_process(COMMAND mv ../libs/assimp-4.1.0 ../libs/assimp)
	execute_process(COMMAND rm ../libs/assimp.zip)
endif()

if (NOT EXISTS ../libs/glm)
	message(STATUS "Downloading glm...")
	file(DOWNLOAD https://github.com/g-truc/glm/releases/download/0.9.9.5/glm-0.9.9.5.zip ../libs/glm.zip)
	execute_process(COMMAND unzip -q ../libs/glm.zip -d ../libs/)
	execute_process(COMMAND mv ../libs/glm-0.9.9.5 ../libs/glm)
	execute_process(COMMAND rm ../libs/glm.zip)
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

add_subdirectory(libs/assimp)

include_directories(includes/)
include_directories(includes/GLScene/)
include_directories(CLDevice/includes/)

include_directories(libs/assimp/include )
include_directories(libs/glew/include)
include_directories(libs/glm)

include_directories(${OpenCL_INCLUDE_DIRS})


add_library(GLScene STATIC ${SOURCE_FILES})
target_link_libraries(GLScene assimp ${OPENGL_LIBRARIES} ${OpenCL_LIBRARY})
