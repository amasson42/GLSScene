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
    _background(0),
    _animatables()
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
    _background(copy._background),
    _animatables()
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
        _animatables.clear();
        return *this;
    }

}
