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
        
		Node *_rootNode; // shared
		Node *_cameraNode; // weak
        glm::vec2 _size;
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
		
        // Rendering
		void renderInContext();
        void sendLightsValueToShader(std::shared_ptr<ShaderProgram> program);
        void subscribeToPostRenderable(IRenderable *renderable, const RenderUniforms& uniforms, float priority);

    };
    
}

#endif /* GLScene_h */
