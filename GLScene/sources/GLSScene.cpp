//
//  GLScene.cpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "GLSScene.hpp"

namespace GLS {
    
//    class Scene {
//        
//        Node *_rootNode;
//        std::weak_ptr<Node> _cameraNode;
//        
//    public:
//        
//        Color backgroundColor;
//        
//        Color lightAmbiant;
//        
//        Vector lightOmniPos;
//        Color lightOmniColor;
//        
//    public:
//        
    Scene::Scene() :
    _rootNode(new Node), _cameraNode(nullptr)
    {
        backgroundColor = Color(0.3, 0.3, 0.3);
        lightAmbiant = Color(0.02, 0.02, 0.02);
        
        useLightOmni = true;
        lightOmniPos = Vector(5, 10, 5);
        lightOmniColor = Color();
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
    
    void Scene::renderInContextWithShader(ShaderProgram& program) {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glClearColor(backgroundColor.r, backgroundColor.g,
                     backgroundColor.b, backgroundColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        Matrix4x4 proj;
        Matrix4x4 view;
        Vector p;
        if (_cameraNode) {
            view = _cameraNode->getWorldTransformMatrix();
            p = view.transform(_cameraNode->position());
            if (_cameraNode->camera() != nullptr)
                proj = _cameraNode->camera()->projectionMatrix();
            else
                proj = Matrix4x4::identity();
        } else {
            view = Matrix4x4::identity();
            p = Vector();
            proj = Matrix4x4::identity();
        }
        
        glUniform3f(program.getLocation("light_ambiant"),
                    lightAmbiant.r, lightAmbiant.g, lightAmbiant.b);
        glUniformMatrix4fv(program.getLocation("projection"), 1, GL_FALSE, proj.m);
        glUniform3f(program.getLocation("view_pos"), p.x, p.y, p.z);
        
        Vector lop = view.transform(lightOmniPos);
        glUniform1i(program.getLocation("omnilight_isactivated"), useLightOmni);
        glUniform3f(program.getLocation("omnilight_position"), lop.x, lop.y, lop.z);
        glUniform3f(program.getLocation("omnilight_color"),
                    lightOmniColor.r, lightOmniColor.g, lightOmniColor.b);
        
        _rootNode->renderInContext(program, view);
    }
    
}
