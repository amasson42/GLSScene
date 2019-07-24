cmake_minimum_required(VERSION 3.5)
project(ft_vox)

if (UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")
endif (UNIX)

if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/libs/)
	execute_process(COMMAND mkdir ${CMAKE_CURRENT_BINARY_DIR}/libs/)
endif()

if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/libs/glfw)
	message(STATUS "Downloading glfw...")
	file(DOWNLOAD https://github.com/glfw/glfw/releases/download/3.3/glfw-3.3.zip ${CMAKE_CURRENT_BINARY_DIR}/libs/glfw.zip)
	execute_process(COMMAND unzip -q ${CMAKE_CURRENT_BINARY_DIR}/libs/glfw.zip -d ${CMAKE_CURRENT_BINARY_DIR}/libs/)
	execute_process(COMMAND mv ${CMAKE_CURRENT_BINARY_DIR}/libs/glfw-3.3 ${CMAKE_CURRENT_BINARY_DIR}/libs/glfw)
	execute_process(COMMAND rm ${CMAKE_CURRENT_BINARY_DIR}/libs/glfw.zip)
endif()

include_directories(includes/)
include_directories(GLScene/includes/)
include_directories(GLScene/includes/GLScene)
include_directories(GLScene/CLDevice/includes/)

find_package(OpenCL REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(${OpenCL_INCLUDE_DIRS})

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(GLScene ${CMAKE_CURRENT_SOURCE_DIR}/GLScene/build)
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/libs/glfw)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/GLScene/build/libs/assimp/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/GLScene/build/build/libs/assimp/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/GLScene/build/libs/glm)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/GLScene/build/libs/glew/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/libs/glfw/include)

# add_subdirectory(GLScene/libs/assimp)

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
target_link_libraries(ft_vox glfw assimp ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES} ${OpenCL_LIBRARY} GLScene)
