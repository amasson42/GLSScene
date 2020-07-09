
#include "CustomSceneControllers/CaveRunning/CaveRunningGame.hpp"

CaveRunningGame::CaveRunningGame(GLS::Scene& scene) :
_scene(&scene)
{
    _currentMaze = std::make_shared<CaveMaze>();
}

CaveRunningGame::~CaveRunningGame() {
    if (!_mazeDisplayedNode.expired())
        _mazeDisplayedNode.lock()->removeFromParent();
    _scene = nullptr;
}

void CaveRunningGame::createNewGame() {
    _currentMaze->generate(CaveMaze::GenerationParameters());
}

void CaveRunningGame::displayCaveScheme() {
    if (!_mazeDisplayedNode.expired()) {
        _mazeDisplayedNode.lock()->removeFromParent();
    }
    GLS::Scene& scene(*_scene);
    CaveMaze& maze(*_currentMaze);

    std::shared_ptr<GLS::Mesh> roomMesh = GLS::Mesh::cube(0.8, 0.8, 0.3);
    roomMesh->setMaterial(std::make_shared<GLS::Material>());
    roomMesh->getMaterial()->diffuse = glm::vec3(0.2, 0.2, 0.7);

    std::shared_ptr<GLS::Mesh> pathMesh = GLS::Mesh::cube(0.3, 0.1, 0.2);
    pathMesh->setMaterial(std::make_shared<GLS::Material>());
    pathMesh->getMaterial()->diffuse = glm::vec3(0.3, 0.3, 0.75);

    std::shared_ptr<GLS::Node> mazeDisplayedNode = std::make_shared<GLS::Node>();
    _mazeDisplayedNode = mazeDisplayedNode;
    for (auto it = maze._rooms.begin(); it != maze._rooms.end(); ++it) {
        std::shared_ptr<GLS::Node> roomNode = std::make_shared<GLS::Node>();
        roomNode->addRenderable(roomMesh);
        roomNode->transform().setPosition(glm::vec3(it->first.x, it->first.y, 0));
        mazeDisplayedNode->addChildNode(roomNode);

        if (it->second.paths & CAVEMAZE_UP) {
            std::shared_ptr<GLS::Node> pathNode = std::make_shared<GLS::Node>();
            pathNode->addRenderable(pathMesh);
            pathNode->transform().setPosition(glm::vec3(0, 0.45, 0));
            pathNode->transform().setEulerAngles(glm::vec3(0, 0, 0));
            roomNode->addChildNode(pathNode);
        }
        if (it->second.paths & CAVEMAZE_DOWN) {
            std::shared_ptr<GLS::Node> pathNode = std::make_shared<GLS::Node>();
            pathNode->addRenderable(pathMesh);
            pathNode->transform().setPosition(glm::vec3(0, -0.45, 0));
            pathNode->transform().setEulerAngles(glm::vec3(0, 0, M_PI));
            roomNode->addChildNode(pathNode);
        }
        if (it->second.paths & CAVEMAZE_LEFT) {
            std::shared_ptr<GLS::Node> pathNode = std::make_shared<GLS::Node>();
            pathNode->addRenderable(pathMesh);
            pathNode->transform().setPosition(glm::vec3(-0.45, 0, 0));
            pathNode->transform().setEulerAngles(glm::vec3(0, 0, M_PI / 2));
            roomNode->addChildNode(pathNode);
        }
        if (it->second.paths & CAVEMAZE_RIGHT) {
            std::shared_ptr<GLS::Node> pathNode = std::make_shared<GLS::Node>();
            pathNode->addRenderable(pathMesh);
            pathNode->transform().setPosition(glm::vec3(0.45, 0, 0));
            pathNode->transform().setEulerAngles(glm::vec3(0, 0, 3 * M_PI / 2));
            roomNode->addChildNode(pathNode);
        }
    }
    {
        std::shared_ptr<GLS::Node> exitNode = std::make_shared<GLS::Node>();
        std::shared_ptr<GLS::Mesh> exitMesh = GLS::Mesh::sphere(0.5);
        exitMesh->setMaterial(std::make_shared<GLS::Material>());
        exitMesh->getMaterial()->diffuse = glm::vec3(0.8, 0.2, 0.3);
        exitNode->addRenderable(exitMesh);
        exitNode->transform().setPosition(glm::vec3(maze._exit.x, maze._exit.y, 0));
        mazeDisplayedNode->addChildNode(exitNode);
    }
    {
        std::shared_ptr<GLS::Node> enterNode = std::make_shared<GLS::Node>();
        std::shared_ptr<GLS::Mesh> enterMesh = GLS::Mesh::sphere(0.3);
        enterMesh->setMaterial(std::make_shared<GLS::Material>());
        enterMesh->getMaterial()->diffuse = glm::vec3(0.2, 0.8, 0.3);
        enterNode->addRenderable(enterMesh);
        enterNode->transform().setPosition(glm::vec3(0, 0, 0));
        mazeDisplayedNode->addChildNode(enterNode);
    }
    scene.rootNode()->addChildNode(mazeDisplayedNode);
}