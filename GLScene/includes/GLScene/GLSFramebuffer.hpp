//
//  GLSFramebuffer.hpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSFramebuffer_h
#define GLSFramebuffer_h

#include "GLScene.hpp"

namespace GLS {
    
    // TODO: allow us to change the fragment shader to create custom post-processing effects

    class Framebuffer {
        GLuint _framebuffer;
        std::shared_ptr<Texture> _colorTexture;

        static GLuint _rectbuffer;

        Framebuffer();
        Framebuffer(const Framebuffer& copy);
        Framebuffer& operator=(const Framebuffer& copy);

        public:
        
        class CreationException : public std::exception {
            public:
            const char *what() const throw();
        };
        
        Framebuffer(GLsizei width, GLsizei height,
            GLint format = GL_RGB, GLenum type = GL_UNSIGNED_BYTE,
            GLenum attachment = GL_COLOR_ATTACHMENT0) throw(CreationException);

        virtual ~Framebuffer();

        GLsizei width() const;
        GLsizei height() const;

        std::shared_ptr<Texture> texture();

        void bind() const;
        void unbind() const;

        void renderInContext();

    };

}

#endif /* GLSFramebuffer_h */
