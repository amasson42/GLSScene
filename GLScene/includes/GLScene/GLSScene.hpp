//
//  GLSScene.hpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSScene_h
#define GLSScene_h

#include "GLSStructs.hpp"
#include "GLSNode.hpp"
#include "GLSIAnimatable.hpp"
#include "GLSShader.hpp"
#include "GLSSkybox.hpp"
#include "GLSFramebuffer.hpp"
#include "GLSLight.hpp"

namespace GLS {
    
    class Scene {

        glm::vec2 _size;
        
        std::shared_ptr<Node> _rootNode;

        std::weak_ptr<Node> _cameraNode;
        std::shared_ptr<Skybox> _skybox;
        glm::vec4 _background;

        std::set<std::shared_ptr<IAnimatable> > _animatables;

        // per frame info
        std::vector<Light> _frameLights;
        std::vector<LightCaster> _frameLightCasters;
        void _renderInLightCasterContext(LightCaster& caster);
        void _calculLights();
        
        std::multimap<float, std::pair<IRenderable*, RenderUniforms> > _postRenderables;

    public:
        
        Scene();
        Scene(glm::vec2 size);
        Scene(const Scene& copy);
        virtual ~Scene();
        
        Scene& operator=(const Scene& copy);


        // Scene utilities

        void setSize(glm::vec2 size);
        glm::vec2 getSize() const;
        float getAspect() const;

        void setBackgroundColor(glm::vec4 color);

        // Node utilities

		std::shared_ptr<Node> rootNode();
		const std::shared_ptr<Node> rootNode() const;

		std::weak_ptr<Node> cameraNode() const;
		void setCameraNode(std::weak_ptr<Node> node);

        std::shared_ptr<Skybox> skybox() const;
        void setSkybox(std::shared_ptr<Skybox> skybox);
		
        // Rendering

        RenderUniforms makeUniforms() const;
		void renderInContext(std::shared_ptr<Framebuffer> framebuffer = nullptr);
        void sendLightsValueToShader(std::shared_ptr<ShaderProgram> program);
        void subscribeToPostRenderable(IRenderable *renderable, const RenderUniforms& uniforms, float priority);

        // Animation

        void addAnimatable(std::shared_ptr<IAnimatable> animatable);
        void removeAnimatable(std::shared_ptr<IAnimatable> animatable);
        void updateAnimations(float deltaTime);

    };
    
}

#endif /* GLScene_h */
