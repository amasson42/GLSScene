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

    std::string Node::globalName() const {
        if (_parent.expired()) {
            return _name;
        } else {
            return _parent.lock()->globalName() + "/" + _name;
        }
    }
    
    void Node::setName(std::string name) {
        if (std::find(name.begin(), name.end(), '/') == name.end()) {
            _name = name;
        } else {
            throw std::runtime_error("GLS::Node names can't contains character '/'");
        }
    }

    Transform& Node::transform() {
        return _transform;
    }

    const Transform& Node::transform() const {
        return _transform;
    }

    void Node::setTransform(const Transform& transform) {
        _transform = transform;
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

    const glm::mat4 Node::getParentNodeRelativeTransformMatrix(std::shared_ptr<Node> parent) {
        if (this == parent.get())
            return glm::mat4(1);
        if (_parent.expired())
            return glm::inverse(parent->getWorldTransformMatrix()) * getTransformMatrix();
        return _parent.lock()->getParentNodeRelativeTransformMatrix(parent) * getTransformMatrix();
    }

    const glm::mat4 Node::getParentNodeRelativeTransformMatrix(const std::shared_ptr<Node> parent) const {
        if (this == parent.get())
            return glm::mat4(1);
        if (_parent.expired())
            return glm::inverse(parent->getWorldTransformMatrix()) * getTransformMatrix();
        return _parent.lock()->getParentNodeRelativeTransformMatrix(parent) * getTransformMatrix();
    }

    bool Node::isActive() const {
        return _active;
    }

    void Node::setActive(bool active) {
        _active = active;
    }

    bool Node::hasRenderable() const {
        return !_renderables.empty();
    }

    const std::vector<std::shared_ptr<IRenderable> >& Node::renderables() const {
        return _renderables;
    }

    std::vector<std::shared_ptr<IRenderable> >& Node::renderables() {
        return _renderables;
    }

    void Node::addRenderable(std::shared_ptr<IRenderable> renderable) {
        _renderables.push_back(renderable);
    }

    void Node::removeRenderableAt(size_t i) {
        if (i < _renderables.size() - 1) {
            _renderables.erase(_renderables.begin() + i);
        } else if (i == _renderables.size() - 1) {
            _renderables.pop_back();
        }
    }

    bool Node::hasAnimatable() const {
        return !_animatables.empty();
    }

    const std::vector<std::shared_ptr<IAnimatable> >& Node::animatables() const {
        return _animatables;
    }

    std::vector<std::shared_ptr<IAnimatable> >& Node::animatables() {
        return _animatables;
    }

    void Node::addAnimatable(std::shared_ptr<IAnimatable> animatable) {
        _animatables.push_back(animatable);
    }

    void Node::removeAnimatable(size_t i) {
        _animatables.erase(_animatables.begin() + i);
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

    bool Node::hasCamera() const {
        return _camera != nullptr;
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

    bool Node::hasLight() const {
        return _light != nullptr;
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

    void Node::sendToFlux(std::ostream& flux, std::string linePrefix) const {
        using ::operator<<;
        flux << linePrefix << "┬─ [Node] " << (_active ? " " : "X") << " \"" << _name << "\" - " << this << std::endl;
        flux << linePrefix << "│ " << _transform << std::endl;
        if (hasCamera())
            flux << linePrefix << "│ camera: " << _camera.get() << std::endl;
        if (hasLight())
            flux << linePrefix << "│ light: " << _light.get() << std::endl;
        if (hasRenderable()) {
            flux << linePrefix << "│ renderables: [" << std::endl;
            for (size_t i = 0; i < _renderables.size(); i++) {
                _renderables[i]->sendToFlux(flux, linePrefix + "│ > ");
            }
            flux << linePrefix << "│ ]" << std::endl;
        }
        if (_childs.size() > 0) {
            flux << linePrefix << "│ childs: [" << std::endl;
            for (size_t i = 0; i < _childs.size(); i++) {
                _childs[i]->sendToFlux(flux, linePrefix + "|   ");
            }
            flux << linePrefix << "│ ]" << std::endl;
        }
        flux << linePrefix << "└─" << std::endl;
    }
    // │├─┬└

}

