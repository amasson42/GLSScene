//
//  GLSScene.hpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSScene_h
#define GLSScene_h

#include "GLScene.hpp"

namespace GLS {
    
    class Scene {

        glm::vec2 _size;
        
		Node *_rootNode;
		Node *_cameraNode;
        // std::unique_ptr<Node> _rootNode;
        // std::weak_ptr<Node> _cameraNode;
        std::shared_ptr<Skybox> _skybox;
        glm::vec4 _background;

        std::vector<Light> _frameLights;
        void _calculLights();
        
        // post rendering
        std::multimap<float, std::pair<IRenderable*, RenderUniforms> > _postRenderables;

    public:
        
        Scene();
        Scene(const Scene& copy);
        virtual ~Scene();
        
        Scene& operator=(const Scene& copy);
        
		Node& rootNode();
		const Node& rootNode() const;
		
		Node *cameraNode() const;
		void setCameraNode(Node& node);

        std::shared_ptr<Skybox> skybox() const;
        void setSkybox(std::shared_ptr<Skybox> skybox);
		
        // Rendering
		void renderInContext();
        void sendLightsValueToShader(std::shared_ptr<ShaderProgram> program);
        void subscribeToPostRenderable(IRenderable *renderable, const RenderUniforms& uniforms, float priority);

    };
    
}

#endif /* GLScene_h */
