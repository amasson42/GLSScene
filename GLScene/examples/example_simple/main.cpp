
#include "GLScene.hpp"

int main(int argc, char const *argv[]) {
    (void)argc;
    (void)argv;
    GLS::glsInit(nullptr);
    std::cout << "Inside !" << std::endl;
    GLS::glsDeinit();
    return 0;
}