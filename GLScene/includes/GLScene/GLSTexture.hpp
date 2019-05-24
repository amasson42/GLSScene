//
//  GLSTexture.hpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSTexture_h
#define GLSTexture_h

#include "GLScene.hpp"

namespace GLS {
    
    class Texture {
        
        GLsizei _width;
        GLsizei _height;
        GLuint _buffer;
        
        Texture();
        Texture(const Texture& copy);
        Texture& operator=(const Texture& copy);
        
    public:
        
        class LoadingException : public std::exception {
            public:
            const char* what() const throw();
        };

        class CreationException : public std::exception {
            public:
            const char* what() const throw();
        };

        Texture(GLsizei width, GLsizei height, GLint format = GL_RGB, GLenum type = GL_UNSIGNED_BYTE) throw(CreationException);
        Texture(std::string path) throw(CreationException, LoadingException);
        virtual ~Texture();
        
        GLuint buffer() const;
        void setParameter(GLenum pname, GLint param);
        void setParameters(std::vector<GLenum> pnames, std::vector<GLint> params);

        GLsizei width() const;
        GLsizei height() const;

    };
    
}

#endif /* GLSTexture_h */
