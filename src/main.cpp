#include "Game.h"
#include <iostream>

int main(int argc, char** argv) {
    std::cout << "====================================" << std::endl;
    std::cout << "    CHRONO GUARDIAN" << std::endl;
    std::cout << "    3D Puzzle Platformer" << std::endl;
    std::cout << "====================================" << std::endl;
    std::cout << std::endl;

    Game game;

    if (!game.init(argc, argv)) {
        std::cerr << "Failed to initialize game!" << std::endl;
        return -1;
    }

    game.run();

    return 0;
}
