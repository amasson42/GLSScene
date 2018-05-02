//
//  GLSNode.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSNode.hpp"
#include <iostream>

namespace GLS {
    
    Node::Node() :
    _name("empty_node"),
    _transform(),
    _parent(nullptr), _childs(),
    _camera(nullptr), _renderables()
    {

    }
    
    Node::Node(const Node& copy) :
    _name(copy._name),
    _transform(copy._transform),
    _parent(nullptr), _childs(),
    _camera(copy._camera), _renderables(copy._renderables)
    {
        for (size_t i = 0; i < copy._childs.size(); i++)
            addChildNode(std::make_shared<Node>(*copy._childs[i]));
    }
    
    Node::~Node() {
        
    }
    
    Node& Node::operator=(const Node& copy) {
        _name = copy._name;
        _transform = copy._transform;
        _parent = nullptr;
        _camera = copy._camera;
        _renderables = copy._renderables;
        for (size_t i = 0; i < copy._childs.size(); i++)
            addChildNode(std::make_shared<Node>(*copy._childs[i]));
        return *this;
    }
    
    std::string Node::name() const {
        return _name;
    }
    
    void Node::setName(std::string name) {
        _name = name;
    }
    
    
    // Transformation
    
    Transform& Node::transform() {
        return _transform;
    }

    const Transform& Node::transform() const {
        return _transform;
    }

    const glm::mat4& Node::getTransformMatrix() {
        return _transform.matrix();
    }

    const glm::mat4 Node::getTransformMatrix() const {
        return _transform.matrix();
    }

    const glm::mat4 Node::getWorldTransformMatrix() {
        if (_parent)
            return _parent->getWorldTransformMatrix() * getTransformMatrix();
        else
            return getTransformMatrix();
    }
    
    const glm::mat4 Node::getWorldTransformMatrix() const {
        if (_parent)
            return _parent->getWorldTransformMatrix() * getTransformMatrix();
        else
            return getTransformMatrix();
    }

    // Hierarchy
    
    std::vector<std::shared_ptr<Node> >& Node::childNodes() {
        return _childs;
    }
    
    void Node::addChildNode(std::shared_ptr<Node> node) {
        _childs.push_back(node);
    }
    
    void Node::removeChildNode(Node *node) {
        std::vector<std::shared_ptr<Node> >::iterator it;
        for (it = _childs.begin(); it != _childs.end(); ++it)
            if (&(**it) == node)
                return static_cast<void>(_childs.erase(it));
    }
    
    void Node::removeFromParent() {
        if (_parent) {
            _parent->removeChildNode(this);
        }
    }
    
    
    // Node functionalities
    
    const std::vector<std::shared_ptr<IRenderable> >& Node::renderables() const {
        return _renderables;
    }

    void Node::addRenderable(std::shared_ptr<IRenderable> renderable) {
        _renderables.push_back(renderable);
    }

    void Node::removeRenderableIndex(size_t i) {
        if (i < _renderables.size() - 1) {
            _renderables.erase(_renderables.begin() + i);
        } else if (i == _renderables.size() - 1) {
            _renderables.pop_back();
        }
    }
    
    std::pair<glm::vec3, glm::vec3> Node::getBounds() const {
        // TODO: this shit
        return std::pair<glm::vec3, glm::vec3>();
    }
    
    const std::shared_ptr<const Camera> Node::camera() const {
        return _camera;
    }
    
    std::shared_ptr<Camera> Node::camera() {
        return _camera;
    }
    
    void Node::setCamera(std::shared_ptr<Camera> camera) {
        _camera = camera;
    }
    
    // SOON: Lights
    
    void Node::renderInContext(Scene& scene, const glm::mat4& projection, const glm::mat4& view) {
        _transform.updateMatrix();
        for (size_t i = 0; i < _childs.size(); i++) {
            _childs[i]->renderInContext(scene, projection, view * getTransformMatrix());
        }
        for (size_t i = 0; i < _renderables.size(); i++) {
            _renderables[i]->renderInContext(scene, projection, view, getTransformMatrix());
        }
    }
    
}
