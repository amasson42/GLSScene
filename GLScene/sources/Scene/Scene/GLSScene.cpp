//
//  GLSScene.cpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSScene.hpp"

namespace GLS {
    
    Scene::Scene() :
    _size(100, 100),
    _rootNode(std::make_shared<Node>()),
    _cameraNode(), _skybox(nullptr),
    _background(0.2)
    {
        _rootNode->setName("root");
    }

    Scene::Scene(glm::vec2 size) :
    Scene()
    {
        _size = size;
    }
    
    Scene::Scene(const Scene& copy) :
    _size(copy._size),
    _rootNode(std::make_shared<Node>(*copy.rootNode())),
    _cameraNode(), _skybox(copy._skybox),
    _background(copy._background)
    {

    }
    
    Scene::~Scene() {

    }
    
    Scene& Scene::operator=(const Scene& copy) {
        _size = copy._size;
        *_rootNode = *copy._rootNode;
        _cameraNode = _rootNode;
        _background = copy._background;
        _skybox = copy._skybox;
        return *this;
    }

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
