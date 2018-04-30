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
    _rootNode(new Node), _cameraNode(nullptr)
    {
        backgroundColor = glm::vec4(0.3, 0.3, 0.3, 1.0);
        lightAmbiant = glm::vec3(0.02, 0.02, 0.02);
        
        useLightOmni = true;
        lightOmniPos = glm::vec3(5, 10, 5);
        lightOmniColor = glm::vec4(1);
    }
    
    Scene::Scene(const Scene& copy) :
    _rootNode(new Node(copy.rootNode())), _cameraNode(nullptr)
    {
        backgroundColor = copy.backgroundColor;
        lightAmbiant = copy.lightAmbiant;
        useLightOmni = copy.useLightOmni;
        lightOmniPos = copy.lightOmniPos;
        lightOmniColor = copy.lightOmniColor;
    }
    
    Scene::~Scene() {
        delete _rootNode;
    }
    
    Scene& Scene::operator=(const Scene& copy) {
        _rootNode = copy._rootNode;
        _cameraNode = nullptr;
        backgroundColor = copy.backgroundColor;
        lightAmbiant = copy.lightAmbiant;
        useLightOmni = copy.useLightOmni;
        lightOmniPos = copy.lightOmniPos;
        lightOmniColor = copy.lightOmniColor;
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
    
    void Scene::renderInContext() {
        
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // positions matrices
        glm::mat4 proj;
        glm::mat4 view;
        glm::vec3 p;
        if (_cameraNode) {
            view = _cameraNode->getWorldTransformMatrix();
            p = glm::vec3(view * glm::vec4(_cameraNode->position(), 1));
            if (_cameraNode->camera() != nullptr)
                proj = _cameraNode->camera()->projectionMatrix();
            else
                proj = glm::mat4(1);
        } else {
            view = glm::mat4(1);
            p = glm::vec3(0);
            proj = glm::mat4(1);
        }

        std::shared_ptr<ShaderProgram> program = ShaderProgram::standardShaderProgram();
        program->use();

        // light using
        glUniform3f(program->getLocation("light_ambiant"),
                    lightAmbiant.x, lightAmbiant.y, lightAmbiant.z);
        glUniformMatrix4fv(program->getLocation("projection"), 1, GL_FALSE, glm::value_ptr(proj));
        glUniform3f(program->getLocation("view_pos"), p.x, p.y, p.z);
        
        glm::vec3 lop = glm::vec3(view * glm::vec4(lightOmniPos, 1.0));
        glUniform1i(program->getLocation("omnilight_isactivated"), useLightOmni);
        glUniform3f(program->getLocation("omnilight_position"), lop.x, lop.y, lop.z);
        glUniform3f(program->getLocation("omnilight_color"),
                    lightOmniColor.x, lightOmniColor.y, lightOmniColor.z);
        
        // node renders
        _rootNode->renderInContext(proj, view);
    }
    
}
