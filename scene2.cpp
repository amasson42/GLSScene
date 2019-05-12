
#include "GLScene.hpp"

std::shared_ptr<GLS::Node> newNode() {
    return std::make_shared<GLS::Node>();
}

void loadScene2(GLS::Scene& scene) {

    // just a nanosuit
    auto nanoSuitNode = newNode();
    nanoSuitNode->loadMeshFromFile("../models/nanosuit/nanosuit.obj");
    scene.rootNode().addChildNode(nanoSuitNode);

    // create ground plane

    // create grass on it

    // create beautiful car

    // create some trees

    // create random shit mesh

}
