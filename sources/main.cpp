//
//  main.cpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "AppEnv.hpp"

void printHelp(const char *argv0) {
    std::cout << "usage: " << std::endl
    << argv0 << " -scene [human | particles | shadow | voxel | cave]" << std::endl
    << " [-effect post_processing_effect.glsl]" << std::endl
    << " [-file path/to/file]" << std::endl
    << " [-image path/to/image]" << std::endl
    << " [-count n]" << std::endl
    << " [-leaksloop]" << std::endl
    << std::endl
    << ""
    << std::endl;

    exit(0);
}

int main(int argc, const char * argv[]) {
    bool loopLeaks = false;
    std::vector<std::string> args;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-leaksloop") == 0) {
            loopLeaks = true;
        } else if (strcmp(argv[i], "-help") == 0) {
            printHelp(argv[0]);
        } else {
            args.push_back(argv[i]);
        }
    }
    if (argc == 1)
        printHelp(argv[0]);
    try {
        AppEnv env(args);
        env.loop();
    } catch (std::exception& e) {
        std::cerr << "Program stop with exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    if (loopLeaks) {
        std::cout << "test leaks..." << std::endl;
        while (1);
    }
    return EXIT_SUCCESS;
}
