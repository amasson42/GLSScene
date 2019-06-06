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
    
    class Framebuffer {
        GLuint _framebuffer;
        std::shared_ptr<Texture> _colorTexture;
        std::shared_ptr<ShaderProgram> _program;

        static GLuint _rectbuffer;
        static void _createRectBuffer();
        static void _destroyRectBuffer();
        friend void glsInit();
        friend void glsDeinit();

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

        void setProgram(std::shared_ptr<ShaderProgram> program);

        void renderInContext();


        // Shader Uniforms

        static std::string shaderUniformsVertex();
        static std::string shaderUniformsGeometry();
        static std::string shaderUniformsFragment();

    };

}

#endif /* GLSFramebuffer_h */
