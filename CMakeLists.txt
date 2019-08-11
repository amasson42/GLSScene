cmake_minimum_required(VERSION 3.5)
project(game)

if (UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")
endif (UNIX)

if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/libs/)
	execute_process(COMMAND mkdir ${CMAKE_CURRENT_BINARY_DIR}/libs/)
endif()


if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/libs/nanogui)
	execute_process(COMMAND git clone https://github.com/wjakob/nanogui.git --recursive ${CMAKE_CURRENT_BINARY_DIR}/libs/nanogui)
endif()

include_directories(includes/)
include_directories(includes/CustomSceneControllers/VoxelProcedural/)
include_directories(GLScene/includes/)
include_directories(GLScene/includes/GLScene)
include_directories(GLScene/CLDevice/includes/)

find_package(OpenCL REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(${OpenCL_INCLUDE_DIRS})

set(NANOGUI_BUILD_EXAMPLE OFF CACHE BOOL " " FORCE)
set(NANOGUI_BUILD_PYTHON  OFF CACHE BOOL " " FORCE)
set(NANOGUI_INSTALL       OFF CACHE BOOL " " FORCE)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/libs/nanogui)
add_subdirectory(GLScene ${CMAKE_CURRENT_SOURCE_DIR}/GLScene/build)
# add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/libs/glfw)


include_directories(${NANOGUI_EXTRA_INCS})
add_definitions(${NANOGUI_EXTRA_DEFS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/GLScene/build/libs/assimp/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/GLScene/build/build/libs/assimp/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/GLScene/build/libs/glm)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/libs/nanogui/include)

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
)

if (WIN32)
	set(SOURCE_FILES ${SOURCE_FILES} ${CMAKE_CURRENT_BINARY_DIR}/libs/nanogui/ext/glad/src/glad.c)

endif()

add_executable(game ${SOURCE_FILES})
target_link_libraries(game nanogui assimp ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES} ${OpenCL_LIBRARY} GLScene)
