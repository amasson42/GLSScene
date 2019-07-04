cmake_minimum_required(VERSION 3.5)
project(ft_vox)

if (UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")
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

include_directories(includes/)
include_directories(GLScene/includes/)
include_directories(GLScene/includes/GLScene)
include_directories(GLScene/CLDevice/includes/)

find_package(OpenCL REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(GLScene/libs/glfw/include)
include_directories(GLScene/libs/assimp/include)
include_directories(GLScene/libs/glm)
include_directories(GLScene/libs/glew/include)
include_directories(${OpenCL_INCLUDE_DIRS})

add_subdirectory(GLScene/libs/glfw)
add_subdirectory(GLScene/libs/assimp)

# find_library(GLSCENE_LIBRARY GLScene)

link_directories(${OpenCL_LIBRARY})
link_directories(build)
# link_directories(GLScene/build/Debug)
# link_directories(${OpenCL_LIBRARY} GLScene/libs/glfw/build/src/Release/)
# link_directories(GLScene/build/Debug)

file(GLOB_RECURSE SOURCE_FILES
    "sources/*.cpp"
    "includes/*.hpp"
	GLScene/libs/glew/src/glew.c
)

add_executable(ft_vox ${SOURCE_FILES})
target_link_libraries(ft_vox glfw assimp ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES} ${OpenCL_LIBRARY} GLScened)
