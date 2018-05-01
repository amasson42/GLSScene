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
        std::vector<std::shared_ptr<IRenderable> > _renderables;
        
    public:
        
        Node();
        Node(const Node& copy);
        virtual ~Node();
        
        Node& operator=(const Node& copy);
        
        std::string name() const;
        void setName(std::string name);

        
        // Transformation
        
        const glm::vec3& position() const;
        void setPosition(const glm::vec3& position);
        
        const glm::quat& rotation() const;
        void setRotation(const glm::quat& rotation);
        
        const glm::vec3& scale() const;
        void setScale(const glm::vec3& scale);
        
        const glm::mat4& getTransformMatrix();
        const glm::mat4 getTransformMatrix() const;
        const glm::mat4 getWorldTransformMatrix();
        const glm::mat4 getWorldTransformMatrix() const;
        void updateTransformMatrix();
        
        
        // Hierarchy
        
        std::vector<std::shared_ptr<Node> >& childNodes();
        void addChildNode(std::shared_ptr<Node> node);
        void removeChildNode(Node *node);
        
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
