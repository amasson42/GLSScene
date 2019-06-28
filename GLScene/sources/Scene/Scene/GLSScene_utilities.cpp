//
//  GLSScene.cpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSScene.hpp"

namespace GLS {

    void Scene::setSize(glm::vec2 size) {
        _size = size;
    }

    glm::vec2 Scene::getSize() const {
        return _size;
    }

    float Scene::getAspect() const {
        return _size.x / _size.y;
    }

    std::shared_ptr<Node> Scene::rootNode() {
        return _rootNode;
    }
    
    const std::shared_ptr<Node> Scene::rootNode() const {
        return _rootNode;
    }
    
    std::weak_ptr<Node> Scene::cameraNode() const {
        return _cameraNode;
    }
    
    void Scene::setCameraNode(std::weak_ptr<Node> node) {
        _cameraNode = node;
    }

    std::shared_ptr<Skybox> Scene::skybox() const {
        return _skybox;
    }

    void Scene::setSkybox(std::shared_ptr<Skybox> skybox) {
        _skybox = skybox;
    }

}
