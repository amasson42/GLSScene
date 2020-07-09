
#pragma once

#include "GLScene.hpp"

#define CAVEMAZE_NONE 0
#define CAVEMAZE_UP 1
#define CAVEMAZE_DOWN 2
#define CAVEMAZE_LEFT 4
#define CAVEMAZE_RIGHT 8

namespace glm {
    bool operator<(const ivec2& lhs, const ivec2& rhs);
}

class CaveMaze {
    public:

    struct Room {
        int paths;

        int themeandshits;

        Room(int paths = CAVEMAZE_NONE);

        static int opposedPath(int path);
        static glm::ivec2 moveVector(int path);
    };

    struct GenerationParameters {

    };

    CaveMaze();
    CaveMaze(const CaveMaze& copy);
    virtual ~CaveMaze();

    CaveMaze& operator=(const CaveMaze& copy);

    void reset();
    void generate(GenerationParameters params);

    public: // private
    std::map<glm::ivec2, Room> _rooms;
    glm::ivec2 _exit;
    // a map contains rooms places at locations allowing us to get to adjacent locations

};
