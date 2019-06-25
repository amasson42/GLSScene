//
//  GLSSkybox.hpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSSkybox_h
#define GLSSkybox_h

#include "GLScene.hpp"

namespace GLS {

    class Skybox : public IRenderable {
        GLuint _textureid;

        static GLuint _cubebuffer;
        static void _createCubeBuffer();
        static void _destroyCubeBuffer();
        friend void glsInit();
        friend void glsDeinit();

    public:

        class LoadingException : public std::exception {
        public:
            const char* what() const throw();
        };

        class CreationException : public std::exception {
        public:
            const char* what() const throw();
        };

        Skybox(std::vector<std::string> faces);

        virtual ~Skybox();


        // Rendering

        virtual void renderInContext(Scene& scene, const RenderUniforms& uniforms);
        virtual std::pair<glm::vec3, glm::vec3> getBounds(glm::mat4 transform = glm::mat4(1)) const;


        // Shader uniforms

        static std::string shaderUniformsVertex();
        static std::string shaderUniformsGeometry();
        static std::string shaderUniformsFragment();

    };

}

#endif /* GLSSkybox_h */
