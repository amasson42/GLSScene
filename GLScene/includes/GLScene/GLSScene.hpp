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
		
        Color backgroundColor;
        
		Color lightAmbiant;
		
		bool useLightOmni;
        Vector lightOmniPos;
        Color lightOmniColor;
        
    public:
        
        Scene();
        Scene(const Scene& copy);
        virtual ~Scene();
        
        Scene& operator=(const Scene& copy);
        
		Node& rootNode();
		const Node& rootNode() const;
		
		Node *cameraNode() const;
		void setCameraNode(Node& node);
		
		void renderInContextWithShader(ShaderProgram& program);
        
    };
    
}

#endif /* GLScene_h */
