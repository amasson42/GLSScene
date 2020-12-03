//
//  GLSRenderable.hpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSIRenderable_h
#define GLSIRenderable_h

#include "GLSStructs.hpp"
#include "GLSShader.hpp"

namespace GLS {
    
    class Scene;

    class IRenderable {
    
    protected:
        bool _castShadow;
        
    public:

        IRenderable();
        IRenderable(const IRenderable& copy);
        virtual ~IRenderable();

        IRenderable& operator=(const IRenderable& copy);

        // Must implement

        virtual void renderInContext(Scene& scene, const RenderUniforms& uniforms) = 0;

        // Optional features

        void setCastShadow(bool v);
        bool castShadow() const;

        virtual std::pair<glm::vec3, glm::vec3> getBounds(glm::mat4 transform = glm::mat4(1)) const;

        virtual void renderInDepthContext(Scene& scene, const RenderUniforms& uniforms);
        virtual void postRenderInContext(Scene& scene, const RenderUniforms& uniforms, float priority);

        virtual void sendToFlux(std::ostream& flux, std::string linePrefix, std::string firstPrefix, std::string lastPrefix) const;

        static std::string shaderUniformsVertex();
        static std::string shaderUniformsGeometry();
        static std::string shaderUniformsFragment();

    };

}

#endif /* GLSIRenderable_h */
