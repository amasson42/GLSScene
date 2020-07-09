
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

int CaveMaze::Room::opposedPath(int path) {
    switch (path) {
        case CAVEMAZE_UP:       return CAVEMAZE_DOWN;
        case CAVEMAZE_DOWN:     return CAVEMAZE_UP;
        case CAVEMAZE_LEFT:     return CAVEMAZE_RIGHT;
        case CAVEMAZE_RIGHT:    return CAVEMAZE_LEFT;
        default: return CAVEMAZE_NONE;
    }
}

glm::ivec2 CaveMaze::Room::moveVector(int path) {
    switch (path) {
        case CAVEMAZE_UP:       return glm::ivec2(0, 1);
        case CAVEMAZE_DOWN:     return glm::ivec2(0, -1);
        case CAVEMAZE_LEFT:     return glm::ivec2(-1, 0);
        case CAVEMAZE_RIGHT:    return glm::ivec2(1, 0);
        default: return glm::ivec2(0, 0);
    }
}

CaveMaze::CaveMaze() :
    _rooms()
{
    reset();
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

void CaveMaze::reset() {
    _rooms.clear();
    _rooms[glm::ivec2(0, 0)] = Room();
    _exit = glm::ivec2(0, 0);
}

static glm::ivec2 _addRoom(std::map<glm::ivec2, CaveMaze::Room>& rooms, glm::ivec2 pos, int path) {
    rooms[pos].paths |= path;
    glm::ivec2 targetPos = pos + CaveMaze::Room::moveVector(path);
    auto targetRoom = rooms.find(targetPos);
    if (targetRoom == rooms.end()) {
        rooms[targetPos] = CaveMaze::Room(CaveMaze::Room::opposedPath(path));
    } else {
        targetRoom->second.paths |= CaveMaze::Room::opposedPath(path);
    }
    return targetPos;
}

void CaveMaze::generate(GenerationParameters params) {
    reset();
    glm::ivec2 diggerPos(0, 0);
    diggerPos = _addRoom(_rooms, diggerPos, CAVEMAZE_UP);
    diggerPos = _addRoom(_rooms, diggerPos, CAVEMAZE_UP);
    diggerPos = _addRoom(_rooms, diggerPos, CAVEMAZE_UP);
    _addRoom(_rooms, diggerPos, CAVEMAZE_LEFT);
    diggerPos = _addRoom(_rooms, diggerPos, CAVEMAZE_RIGHT);
    diggerPos = _addRoom(_rooms, diggerPos, CAVEMAZE_RIGHT);
    diggerPos = _addRoom(_rooms, diggerPos, CAVEMAZE_DOWN);
    diggerPos = _addRoom(_rooms, diggerPos, CAVEMAZE_RIGHT);
    diggerPos = _addRoom(_rooms, diggerPos, CAVEMAZE_UP);
    diggerPos = _addRoom(_rooms, diggerPos, CAVEMAZE_LEFT);
    diggerPos = _addRoom(_rooms, diggerPos, CAVEMAZE_UP);
    diggerPos = _addRoom(_rooms, diggerPos, CAVEMAZE_UP);
    _addRoom(_rooms, diggerPos, CAVEMAZE_RIGHT);
    diggerPos = _addRoom(_rooms, diggerPos, CAVEMAZE_LEFT);
    diggerPos = _addRoom(_rooms, diggerPos, CAVEMAZE_LEFT);
    diggerPos = _addRoom(_rooms, diggerPos, CAVEMAZE_LEFT);
    diggerPos = _addRoom(_rooms, diggerPos, CAVEMAZE_LEFT);
    _rooms[diggerPos].paths |= CAVEMAZE_UP;
    _exit = diggerPos + glm::ivec2(0, 1);
}
