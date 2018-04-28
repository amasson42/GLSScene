//
//  GLSNode.hpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSNode_h
#define GLSNode_h

#include <string>
#include <vector>
#include "GLScene.hpp"

namespace GLS {
    
    class Node {
        
        std::string _name;
        
        glm::vec3 _position;
        glm::quat _rotation;
        glm::vec3 _scale;
        glm::mat4 _transform;
        bool _transformUpdated;
        
        Node *_parent;
        std::vector<std::shared_ptr<Node> > _childs;
        
        std::shared_ptr<Camera> _camera;
        std::shared_ptr<Mesh> _mesh;
        
    public:
        
        Node();
        Node(const Node& copy);
        virtual ~Node();
        
        Node& operator=(const Node& copy);
        
        std::string name() const;
        void setName(std::string name);

        
        // Transformation
        
        glm::vec3 position() const;
        void setPosition(glm::vec3 position);
        
        glm::quat rotation() const;
        void setRotation(glm::quat rotation);
        
        glm::vec3 scale() const;
        void setScale(glm::vec3 scale);
        
        glm::mat4 getTransformMatrix();
        glm::mat4 getTransformMatrix() const;
        glm::mat4 getWorldTransformMatrix();
        glm::mat4 getWorldTransformMatrix() const;
        void updateTransformMatrix();
        
        
        // Hierarchy
        
        std::vector<std::shared_ptr<Node> >& childNodes();
        void addChildNode(std::shared_ptr<Node> node);
        void removeChildNode(Node *node);
        
        void removeFromParent();
        
        
        // Node functionalities
        
        const std::shared_ptr<const Mesh> mesh() const;
        std::shared_ptr<Mesh> mesh();
        void setMesh(std::shared_ptr<Mesh> mesh);
        
        std::pair<glm::vec3, glm::vec3> getBounds() const;
        
        const std::shared_ptr<const Camera> camera() const;
        std::shared_ptr<Camera> camera();
        void setCamera(std::shared_ptr<Camera> camera);
        
        // SOON: Lights
        
        void renderInContext(ShaderProgram& program, glm::mat4 view);
        
    };
    
}

#endif /* GLSNode_h */
