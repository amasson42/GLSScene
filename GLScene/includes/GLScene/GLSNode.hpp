//
//  GLSNode.hpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSNode_h
#define GLSNode_h

#include "GLScene.hpp"

namespace GLS {
    
    class Node {
        
        std::string _name;
        Transform _transform;
        
        Node *_parent;
        std::vector<std::shared_ptr<Node> > _childs;
        
        std::shared_ptr<Camera> _camera;
        std::vector<std::shared_ptr<IRenderable> > _renderables;
        
    public:
        
        Node();
        Node(const Node& copy);
        virtual ~Node();
        
        Node& operator=(const Node& copy);
        
        std::string name() const;
        void setName(std::string name);

        
        // Transformation
        
        Transform& transform();
        const Transform& transform() const;

        const glm::mat4& getTransformMatrix();
        const glm::mat4 getTransformMatrix() const;
        const glm::mat4 getWorldTransformMatrix();
        const glm::mat4 getWorldTransformMatrix() const;


        // Hierarchy
        
        const std::vector<std::shared_ptr<Node> >& childNodes();
        std::shared_ptr<Node> childNodeAt(size_t i) const;

        void addChildNode(std::shared_ptr<Node> node);
        void removeChildNode(Node *node);
        
        bool hasParentNode(Node* node) const;
        void removeFromParent();
        
        
        // Node functionalities
        
        const std::vector<std::shared_ptr<IRenderable> >& renderables() const;
        void addRenderable(std::shared_ptr<IRenderable> renderable);
        void removeRenderableIndex(size_t i);
        const std::shared_ptr<const Mesh> mesh() const;
        std::shared_ptr<Mesh> mesh();
        void setMesh(std::shared_ptr<Mesh> mesh);
        
        std::pair<glm::vec3, glm::vec3> getBounds() const;
        
        const std::shared_ptr<const Camera> camera() const;
        std::shared_ptr<Camera> camera();
        void setCamera(std::shared_ptr<Camera> camera);
        
        // SOON: Lights
        
        void renderInContext(Scene& scene, const glm::mat4& projection, const glm::mat4& view);
        
    };
    
}

#endif /* GLSNode_h */
