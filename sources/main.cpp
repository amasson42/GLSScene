//
//  main.cpp
//  GLScene
//
//  Created by Arthur Masson on 12/04/2018.
//  Copyright © 2018 Arthur Masson. All rights reserved.
//

#include "AppEnv.hpp"

void printHelp(const char *argv0) {
    std::cout << "usage: " << argv0 << " scene_name [-effect post_processing_effect.glsl] ..." << std::endl;
    AppEnv::printAvailableScenes();
}

int main(int argc, const char * argv[]) {
    bool loopLeaks = false;
    bool helpDisplay = false;
    std::vector<std::string> args;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-leaks") == 0) {
            loopLeaks = true;
        } else if (strcmp(argv[i], "-help") == 0) {
            helpDisplay = true;
        } else {
            args.push_back(argv[i]);
        }
    }
    if (helpDisplay)
        printHelp(argv[0]);
    if (args.empty()) {
        if (!helpDisplay) {
            std::cerr << "invalid number of arguments..." << std::endl;
            std::cerr << "HINT: use " << argv[0] << " -help" << std::endl;
        }
        return EXIT_FAILURE;
    }
    if (helpDisplay)
        args.push_back("-help");
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
