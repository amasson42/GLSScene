
#include "CLDevice.hpp"
#include <iostream>

int main(int argc, char const *argv[]) {
    (void)argc;
    (void)argv;
    std::shared_ptr<CLD::GPUDevice> device = std::make_shared<CLD::GPUDevice>();
    std::cout << "Made it !" << std::endl;
    return 0;
}
