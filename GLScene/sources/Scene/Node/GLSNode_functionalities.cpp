//
//  GLSNode_functionalities.cpp
//  GLScene
//
//  Created by Arthur Masson on 13/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSNode.hpp"

namespace GLS {

    std::string Node::name() const {
        return _name;
    }
    
    void Node::setName(std::string name) {
        _name = name;
    }

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
        if (_parent.expired())
            return getTransformMatrix();
        else
            return _parent.lock()->getWorldTransformMatrix() * getTransformMatrix();
    }
    
    const glm::mat4 Node::getWorldTransformMatrix() const {
        if (_parent.expired())
            return getTransformMatrix();
        else
            return _parent.lock()->getWorldTransformMatrix() * getTransformMatrix();
    }

    bool Node::isActive() const {
        return _active;
    }

    void Node::setActive(bool active) {
        _active = active;
    }

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

    static std::pair<glm::vec3, glm::vec3> mergeBounds(std::pair<glm::vec3, glm::vec3> b1, std::pair<glm::vec3, glm::vec3> b2) {
        std::pair<glm::vec3, glm::vec3> bounds;
        bounds.first = glm::vec3(std::min(b1.first.x, b2.first.x),
                                 std::min(b1.first.y, b2.first.y),
                                 std::min(b1.first.z, b2.first.z));
        bounds.second = glm::vec3(std::max(b1.second.x, b2.second.x),
                                  std::max(b1.second.y, b2.second.y),
                                  std::max(b1.second.z, b2.second.z));
        return bounds;
    }

    std::pair<glm::vec3, glm::vec3> Node::getBounds() const {
        glm::mat4 modelMatrix = getWorldTransformMatrix();
        std::pair<glm::vec3, glm::vec3> bounds;
        for (size_t i = 0; i < _renderables.size(); i++) {
            bounds = mergeBounds(bounds, _renderables[i]->getBounds(modelMatrix));
        }
        for (size_t i = 0; i < _childs.size(); i++) {
            bounds = mergeBounds(bounds, _childs[i]->getBounds());
        }
        return bounds;
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
    
    const std::shared_ptr<const Light> Node::light() const {
        return _light;
    }

    std::shared_ptr<Light> Node::light() {
        return _light;
    }

    void Node::setLight(std::shared_ptr<Light> light) {
        _light = light;
    }

    void Node::getAllLights(std::vector<Light>& container, glm::mat4 parentMatrix) {
        if (!_active)
            return;
        glm::mat4 mat = parentMatrix * getTransformMatrix();
        if (_light != nullptr)
            container.push_back(_light->transformedBy(mat));
        for (size_t i = 0; i < _childs.size(); i++) {
            _childs[i]->getAllLights(container, mat);
        }
    }

}
