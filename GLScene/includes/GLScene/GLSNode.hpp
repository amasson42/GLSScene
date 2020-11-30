//
//  GLSNode.hpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#ifndef GLSNode_h
#define GLSNode_h

#include "GLSStructs.hpp"
#include "GLSIRenderable.hpp"
#include "GLSCamera.hpp"
#include "GLSLight.hpp"
#include "GLSSkeleton.hpp"

namespace GLS {
    
    class Node : public std::enable_shared_from_this<Node> {
        
        std::string _name;
        Transform _transform;

        std::weak_ptr<Node> _parent;
        std::vector<std::shared_ptr<Node> > _childs;

        bool _active;
        std::shared_ptr<Camera> _camera;
        std::shared_ptr<Light> _light;
        std::vector<std::shared_ptr<IRenderable> > _renderables;
        std::shared_ptr<Skeleton> _skeleton;

    public:
        
        Node();
        Node(const Node& copy);
        virtual ~Node();
        
        Node& operator=(const Node& copy);
        
        static std::shared_ptr<Node> loadFromFile(std::string path);
        static std::shared_ptr<Node> loadFromAiScene(const aiScene *scene, std::string directory = "");

        // Transformation
        
        Transform& transform();
        const Transform& transform() const;
        void setTransform(const Transform& transform);

        const glm::mat4& getTransformMatrix();
        const glm::mat4 getTransformMatrix() const;
        const glm::mat4 getWorldTransformMatrix();
        const glm::mat4 getWorldTransformMatrix() const;
        const glm::mat4 getParentNodeRelativeTransformMatrix(std::shared_ptr<Node> parent);
        const glm::mat4 getParentNodeRelativeTransformMatrix(const std::shared_ptr<Node> parent) const;


        // Hierarchy
        
        const std::vector<std::shared_ptr<Node> >& childNodes() const;
        std::shared_ptr<Node> childNodeAt(size_t i) const;
        std::shared_ptr<Node> childNodeNamed(std::string name, bool recursively = false) const;

        void addChildNode(std::shared_ptr<Node> node);
        void removeChildNode(std::shared_ptr<Node> node);
        void removeChildNode(Node* node);
        
        std::shared_ptr<Node> getParentNode() const;
        bool hasParentNode(std::shared_ptr<Node> node) const;
        void removeFromParent();
        
        
        // Node functionalities

        std::string name() const;
        void setName(std::string name);

        bool isActive() const;
        void setActive(bool active);

        bool hasRenderable() const;
        const std::vector<std::shared_ptr<IRenderable> >& renderables() const;
        std::vector<std::shared_ptr<IRenderable> >& renderables();
        void addRenderable(std::shared_ptr<IRenderable> renderable);
        void removeRenderableAt(size_t i);

        std::pair<glm::vec3, glm::vec3> getBounds() const;

        bool hasCamera() const;
        const std::shared_ptr<const Camera> camera() const;
        std::shared_ptr<Camera> camera();
        void setCamera(std::shared_ptr<Camera> camera);

        bool hasLight() const;
        const std::shared_ptr<const Light> light() const;
        std::shared_ptr<Light> light();
        void setLight(std::shared_ptr<Light> light);
        void getAllLights(std::vector<Light>& container, glm::mat4 parentMatrix);

        bool hasSkeleton() const;
        const std::shared_ptr<Skeleton> skeleton() const;
        std::shared_ptr<Skeleton> skeleton();
        void setSkeleton(std::shared_ptr<Skeleton> skeleton);

        void renderInContext(Scene& scene, RenderUniforms uniforms);
        void renderInDepthContext(Scene& scene, RenderUniforms uniforms);

        void sendToFlux(std::ostream& flux, std::string linePrefix) const;

    };

}

#endif /* GLSNode_h */
