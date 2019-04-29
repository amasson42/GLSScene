//
//  GLScene.cpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSScene.hpp"

namespace GLS {
    
    Scene::Scene() :
    _rootNode(new Node), _cameraNode(nullptr),
    _size(100, 100),
    _background(0.2)
    {

    }
    
    Scene::Scene(const Scene& copy) :
    _rootNode(new Node(copy.rootNode())), _cameraNode(nullptr),
    _size(copy._size),
    _background(copy._background)
    {

    }
    
    Scene::~Scene() {
        delete _rootNode;
    }
    
    Scene& Scene::operator=(const Scene& copy) {
        _rootNode = copy._rootNode;
        _cameraNode = nullptr;
        _size = copy._size;
        _background = copy._background;
        return *this;
    }
    
    Node& Scene::rootNode() {
        return *_rootNode;
    }
    
    const Node& Scene::rootNode() const {
        return *_rootNode;
    }
    
    Node *Scene::cameraNode() const {
        return _cameraNode;
    }
    
    void Scene::setCameraNode(Node& node) {
        _cameraNode = &node;
    }

    void Scene::_calculLights() {
        _frameLights.clear();
        _rootNode->_getAllLights(_frameLights, glm::mat4(1));
    }

    void Scene::sendLightsValueToShader(std::shared_ptr<ShaderProgram> program) {
        program->use();
        for (size_t i = 0; i < _frameLights.size() && i < 16; i++) {
            _frameLights[i].sendUniformToShaderProgram(program, static_cast<int>(i));
        }
        glUniform1i(program->getLocation("lights_count"), static_cast<int>(_frameLights.size()));
    }
    
    void Scene::renderInContext() {
        
        glClearColor(_background.x, _background.y, _background.z, _background.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        
        RenderUniforms uniforms;
        if (_cameraNode) {
            uniforms.view = glm::inverse(_cameraNode->getWorldTransformMatrix());
            uniforms.camera_position = glm::vec3(uniforms.view * glm::vec4(_cameraNode->transform().position(), 1));
            if (_cameraNode->camera() != nullptr)
                uniforms.projection = _cameraNode->camera()->projectionMatrix();
        }
        
        std::shared_ptr<ShaderProgram> program = ShaderProgram::standardShaderProgramMesh();
        _calculLights();
        sendLightsValueToShader(program);

        _rootNode->renderInContext(*this, uniforms);
    }
    
}
