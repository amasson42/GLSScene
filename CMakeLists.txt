cmake_minimum_required(VERSION 3.5)
project(ft_vox)

if (UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Werror -Wall -Werror -g")
endif (UNIX)

if (NOT EXISTS ../libs/)
	execute_process(COMMAND mkdir ../libs/)
endif()

if (NOT EXISTS ../libs/glfw)
	message(STATUS "Downloading glfw...")
	file(DOWNLOAD https://github.com/glfw/glfw/releases/download/3.3/glfw-3.3.zip ../libs/glfw.zip)
	execute_process(COMMAND unzip -q ../libs/glfw.zip -d ../libs/)
	execute_process(COMMAND mv ../libs/glfw-3.3 ../libs/glfw)
	execute_process(COMMAND rm ../libs/glfw.zip)
endif()

if (NOT EXISTS ../libs/assimp)
	message(STATUS "Downloading assimp...")
	file(DOWNLOAD https://github.com/assimp/assimp/archive/v4.1.0.zip ../libs/assimp.zip)
	execute_process(COMMAND unzip -q ../libs/assimp.zip -d ../libs/)
	execute_process(COMMAND mv ../libs/assimp-4.1.0 ../libs/assimp)
	execute_process(COMMAND rm ../libs/assimp.zip)
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

if (NOT EXISTS ../libs/glm)
	message(STATUS "Downloading glm...")
	file(DOWNLOAD https://github.com/g-truc/glm/releases/download/0.9.9.5/glm-0.9.9.5.zip ../libs/glm.zip)
	execute_process(COMMAND unzip -q ../libs/glm.zip -d ../libs/)
	execute_process(COMMAND mv ../libs/glm-0.9.9.5 ../libs/glm)
	execute_process(COMMAND rm ../libs/glm.zip)
endif()

add_subdirectory(libs/assimp)
add_subdirectory(libs/glfw)


include_directories(GLScene/includes/)
include_directories(GLScene/CLDevice/includes/)
include_directories(GLScene/libs/glfw/include)
include_directories(libs/assimp/include)
include_directories(libs/glm)
include_directories(libs/glew/include)
include_directories(includes/)
include_directories(GLScene/includes/)
include_directories(GLScene/includes/GLScene)
include_directories(GLScene/CLDevice/includes)
include_directories(build/libs/assimp/include)

find_package(OpenCL REQUIRED)
find_package(OpenGL REQUIRED)

# find_library(GLSCENE_LIBRARY GLScene)

include_directories(${OpenCL_INCLUDE_DIRS})
link_directories(${OpenCL_LIBRARY})
# link_directories(GLScene/build/Debug)
link_directories(${OpenCL_LIBRARY} GLScene/libs/glfw/build/src/Release/)
link_directories(GLScene/build/Debug)

file(GLOB_RECURSE SOURCE_FILES
    "sources/*.cpp"
    "includes/*.hpp"
	"GLScene/sources/*.cpp"
	"GLScene/includes/*.hpp"
	"GLScene/CLDevice/sources/*.cpp"
	"GLScene/CLDevice/includes/*.hpp"
)

add_executable(ft_vox ${SOURCE_FILES})
target_link_libraries(ft_vox glfw3 ${GLFW_LIBRARIES} assimp ${OPENGL_LIBRARIES} ${OpenCL_LIBRARY} GLScened)
