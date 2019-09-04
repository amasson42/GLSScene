//
//  GLSTexture.hpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSTexture_h
#define GLSTexture_h

#include "GLSStructs.hpp"
#include "GLSShader.hpp"

namespace GLS {
    
    class Texture {
        
        GLsizei _width;
        GLsizei _height;
        GLuint _buffer;
        
        Texture();
        Texture(const Texture& copy);
        Texture& operator=(const Texture& copy);
        
    public:

        Texture(GLsizei width, GLsizei height, GLint format = GL_RGB, GLenum type = GL_UNSIGNED_BYTE);
        Texture(std::string path);
        virtual ~Texture();

        GLuint buffer() const;
        void sendUniformsToShaderProgram(std::shared_ptr<ShaderProgram> program, std::string uniformName, int textureIndex) const;
        void setParameter(GLenum pname, GLint param);
        void setParameters(std::vector<GLenum> pnames, std::vector<GLint> params);

        GLsizei width() const;
        GLsizei height() const;


    };
    
}

#endif /* GLSTexture_h */
