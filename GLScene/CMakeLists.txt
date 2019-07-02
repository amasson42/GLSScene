# cmake_minimum_required(VERSION 3.5)
# project(GLScene)
# 
# if (UNIX)
# 	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Werror -Wall -Werror -g")
# endif (UNIX)
# 
# add_subdirectory(libs/assimp)
# 
# include_directories(libs/assimp/include)
# include_directories(libs/glm)
# include_directories(includes/)
# include_directories(libs/glew/include)
# include_directories(includes/GLScene)
# include_directories(CLDevice/includes)
# 
# find_package(OpenCL REQUIRED)
# find_package(OpenGL REQUIRED)
# 
# include_directories(${OpenCL_INCLUDE_DIRS})
# # link_directories(${OpenGL_LIBRARY})
# link_directories(CLDevice/build/Debug)
# link_directories(${OpenCL_LIBRARY} libs/glfw/build/src/Release/)
# 
# file(GLOB_RECURSE SOURCE_FILES
#     "sources/*.cpp"
#     "includes/*.hpp"
# 	libs/glew/src/glew.c
# )
# 
# message(${SOURCE_FILES})
# 
# add_library(GLScene SHARED ${SOURCE_FILES})
# target_link_libraries(GLScene glfw3 ${GLFW_LIBRARIES} assimp ${OPENGL_LIBRARIES} ${OpenCL_LIBRARY} CLDevice)








cmake_minimum_required(VERSION 3.5)
project(GLScene)

if (UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Werror -Wall -Werror -g")
endif (UNIX)


include_directories(includes/)
include_directories(includes/GLScene)
include_directories(libs/glew/include)
include_directories(libs/assimp/include)
include_directories(libs/glm)

include_directories(CLDevice/includes)

add_subdirectory(libs/assimp)

find_package(OpenCL REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(${OpenCL_INCLUDE_DIRS})
link_directories(${OpenCL_LIBRARY} libs/glew/build/lib/Release)
link_directories(CLDevice/build/Debug)


file(GLOB_RECURSE SOURCE_FILES
	libs/glew/src/glew.c
    "sources/*.cpp"
    "includes/*.hpp"
)


add_library(GLScene ${SOURCE_FILES})
target_link_libraries(GLScene assimp ${OpenCL_LIBRARY} ${OPENGL_LIBRARIES} CLDevice)
