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
        
        Vector _position;
        Vector _rotation;
        Vector _scale;
        Matrix4x4 _transform;
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
        
        Vector position() const;
        void setPosition(Vector position);
        
        Vector rotation() const;
        void setRotation(Vector rotation);
        
        Vector scale() const;
        void setScale(Vector scale);
        
        Matrix4x4 getTransformMatrix();
        Matrix4x4 getTransformMatrix() const;
        Matrix4x4 getWorldTransformMatrix();
        Matrix4x4 getWorldTransformMatrix() const;
        void updateTransformMatrix();
        
        
        // Hierarchy
        
        void addChildNode(std::shared_ptr<Node> node);
        void removeChildNode(Node *node);
        
        void removeFromParent();
        
        
        // Node functionalities
        
        const std::shared_ptr<const Mesh> mesh() const;
        std::shared_ptr<Mesh> mesh();
        void setMesh(std::shared_ptr<Mesh> mesh);
        
        std::pair<Vector, Vector> getBounds() const;
        
        const std::shared_ptr<const Camera> camera() const;
        std::shared_ptr<Camera> camera();
        void setCamera(std::shared_ptr<Camera> camera);
        
        // SOON: Lights
        
        void renderInContext(ShaderProgram& program, Matrix4x4 view);
        
    };
    
}

#endif /* GLSNode_h */
