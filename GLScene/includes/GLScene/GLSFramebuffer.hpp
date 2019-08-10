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
    
    class Framebuffer : public IRenderable {

        GLuint _framebuffer;
        GLuint _renderbuffer;
        std::shared_ptr<Texture> _colorTexture;
        std::shared_ptr<ShaderProgram> _program;

        static GLuint _rectbuffer;
        static void _createRectBuffer();
        static void _destroyRectBuffer();
        friend void glsInit(void* addr);
        friend void glsDeinit();

        Framebuffer();
        Framebuffer(const Framebuffer& copy);
        Framebuffer& operator=(const Framebuffer& copy);

        public:
        
        Framebuffer(GLsizei width, GLsizei height,
            bool createRenderbuffer = true,
            GLint format = GL_RGB, GLenum type = GL_UNSIGNED_BYTE,
            GLenum attachment = GL_COLOR_ATTACHMENT0);

        virtual ~Framebuffer();


        // Texture utilities

        GLsizei width() const;
        GLsizei height() const;

        std::shared_ptr<Texture> texture();

        std::pair<glm::vec3, glm::vec3> getBounds(glm::mat4 transform = glm::mat4(1)) const;

        // Rendering

        void bind() const;
        void unbind() const;

        void setProgram(std::shared_ptr<ShaderProgram> program);

        void renderInContext(Scene& scene, const RenderUniforms& uniforms);


        // Shader Uniforms

        static std::string shaderUniformsVertex();
        static std::string shaderUniformsGeometry();
        static std::string shaderUniformsFragment();

    };

}

#endif /* GLSFramebuffer_h */
