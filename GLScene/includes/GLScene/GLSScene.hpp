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
        
		Node *_rootNode;
		Node *_cameraNode;
        
	public:
		
        // TODO: use the node to set the lights
        glm::vec4 backgroundColor;
        
		glm::vec3 lightAmbiant;
		
		bool useLightOmni;
        glm::vec3 lightOmniPos;
        glm::vec3 lightOmniColor;
        
    public:
        
        Scene();
        Scene(const Scene& copy);
        virtual ~Scene();
        
        Scene& operator=(const Scene& copy);
        
		Node& rootNode();
		const Node& rootNode() const;
		
		Node *cameraNode() const;
		void setCameraNode(Node& node);
		
        void sendLightsValueToShader(std::shared_ptr<ShaderProgram> program);

		void renderInContext();
        
    };
    
}

#endif /* GLScene_h */
