//
//  GLScene.hpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLScene_h
#define GLScene_h

// TODO: why do we have to do this shit
// #define SCHOOL_DUMPS

#define _USE_MATH_DEFINES
#include <cmath>

#include <iostream>
#include <fstream>
#include <sstream>
#include <exception>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <set>
#include <algorithm>
#include <future>
#include <sys/stat.h>
#include <iomanip>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#ifdef __APPLE__
# define GL_SILENCE_DEPRECATION
# include <OpenGL/gl3.h>
#elif defined _WIN32
# include <Windows.h>
#  include <glad/glad.h>
#else
# include <gl/gl.h>
#endif

#include "GLScene/GLSStructs.hpp"
#include "GLScene/GLSTransform.hpp"

#include "GLScene/GLSAnimator.hpp"

#include "GLScene/GLSTexture.hpp"

#include "GLScene/GLSIRenderable.hpp"
#include "GLScene/GLSIAnimatable.hpp"
#include "GLScene/GLSFramebuffer.hpp"
#include "GLScene/GLSMesh.hpp"
#include "GLScene/GLSSkinnedMesh.hpp"
#include "GLScene/GLSVoxelChunk.hpp"
#include "GLScene/GLSSkybox.hpp"
#include "GLScene/GLSParticleSystem.hpp"

#include "GLScene/GLSCamera.hpp"
#include "GLScene/GLSLight.hpp"

#include "GLScene/GLSShader.hpp"
#include "GLScene/GLSNode.hpp"
#include "GLScene/GLSScene.hpp"

#endif /* GLScene_h */
