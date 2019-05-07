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
    _rootNode(new Node),
    _cameraNode(nullptr), _skybox(nullptr),
    _background(0.2)
    {

    }
    
    Scene::Scene(const Scene& copy) :
    _size(copy._size),
    _rootNode(new Node(copy.rootNode())),
    _cameraNode(nullptr), _skybox(copy._skybox),
    _background(copy._background)
    {

    }
    
    Scene::~Scene() {
        delete _rootNode;
    }
    
    Scene& Scene::operator=(const Scene& copy) {
        _size = copy._size;
        _rootNode = copy._rootNode;
        _cameraNode = nullptr;
        _background = copy._background;
        _skybox = copy._skybox;
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

    std::shared_ptr<Skybox> Scene::skybox() const {
        return _skybox;
    }

    void Scene::setSkybox(std::shared_ptr<Skybox> skybox) {
        _skybox = skybox;
    }

    void Scene::_calculLights() {
        _rootNode->_getAllLights(_frameLights, glm::mat4(1));
    }

    void Scene::sendLightsValueToShader(std::shared_ptr<ShaderProgram> program) {
        program->use();
        for (size_t i = 0; i < _frameLights.size() && i < 16; i++) {
            _frameLights[i].sendUniformToShaderProgram(program, static_cast<int>(i));
        }
        glUniform1i(program->getLocation("lights_count"), static_cast<int>(_frameLights.size()));
    }

    typedef std::pair<IRenderable*, RenderUniforms> RenderAndUniforms;

    void Scene::renderInContext() {
        
        glClearColor(_background.x, _background.y, _background.z, _background.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        _frameLights.clear();
        _postRenderables.clear();

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
        
        std::multimap<float, RenderAndUniforms>::iterator itr;
        for (itr = _postRenderables.begin(); itr != _postRenderables.end(); ++itr) {
            IRenderable *renderable = itr->second.first;
            RenderUniforms uniforms = itr->second.second;
            float priority = itr->first;
            renderable->postRenderInContext(*this, uniforms, priority);
        }

        if (_skybox != nullptr) {
            _skybox->renderInContext(*this, uniforms);
        }
    }
    
    void Scene::subscribeToPostRenderable(IRenderable* renderable, const RenderUniforms& uniforms, float priority) {
        RenderAndUniforms paired(renderable, uniforms);
        _postRenderables.insert(std::pair<float, RenderAndUniforms>(priority, paired));
    }

}
