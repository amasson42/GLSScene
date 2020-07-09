
#pragma once

#include "GLScene.hpp"

#include "CaveMaze.hpp"

/**
 * A cave running scene is the object managing the game system into a previously created scene.
 * As long as this object exist, the scene should be operational and ready to run.
 */
class CaveRunningGame {
    
    GLS::Scene *_scene;

    std::shared_ptr<CaveMaze> _currentMaze;
    std::weak_ptr<GLS::Node> _mazeDisplayedNode;

    public:

    CaveRunningGame(GLS::Scene& scene);
    virtual ~CaveRunningGame();

    void createNewGame();
    void displayCaveScheme();
    
};
