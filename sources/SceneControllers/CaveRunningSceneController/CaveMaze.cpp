
#include "CustomSceneControllers/CaveRunning/CaveMaze.hpp"

namespace glm {
	bool operator<(const ivec2& lhs, const ivec2& rhs) {
        if (lhs.x < rhs.x)
            return true;
        else if (lhs.x == rhs.x)
            return lhs.y < rhs.y;
        return false;
    }
}

CaveMaze::Room::Room(int p) :
    paths(p)
{
    themeandshits = 0;
}

CaveMaze::CaveMaze() :
    _rooms(),
    _exit(0, 1)
{
    _rooms[glm::ivec2(0, 0)] = Room(CAVEMAZE_UP);
}

CaveMaze::CaveMaze(const CaveMaze& copy) :
    _rooms(copy._rooms),
    _exit(copy._exit) {
    
}

CaveMaze::~CaveMaze() {

}

CaveMaze& CaveMaze::operator=(const CaveMaze& copy) {
    _rooms = copy._rooms;
    _exit = copy._exit;
    return *this;
}

void CaveMaze::generate(GenerationParameters params) {
    _rooms[glm::ivec2(0, 0)].paths |= CAVEMAZE_UP;
    _rooms[glm::ivec2(0, 1)] = Room(CAVEMAZE_DOWN | CAVEMAZE_UP);
    _exit = glm::ivec2(0, 2);
}
