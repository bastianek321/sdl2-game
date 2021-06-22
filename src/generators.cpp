#include "generators.hpp"

std::vector<Object> generate_obstacles(Game game)
{
    std::vector<Object> obstacles;
    Object obstacle(0, 80, 200, 100, game); // 1
    obstacles.push_back(obstacle);
    Object obstacle2(280, 0, 100, 270, game); // 2
    obstacles.push_back(obstacle2);
    Object obstacle3(100, 270, 280, 100, game); // 3
    obstacles.push_back(obstacle3);
    Object obstacle4(100, 370, 100, 200, game); // 4
    obstacles.push_back(obstacle4);
    Object obstacle5(280, 475, 300, 100, game); // 5
    obstacles.push_back(obstacle5);
    Object obstacle6(480, 100, 100, 380, game); // 6
    obstacles.push_back(obstacle6);
    Object obstacle7(680, 0, 100, 580, game); // 7
    obstacles.push_back(obstacle7);
    return obstacles;
}

std::vector<Trap> generate_traps(Game game)
{
    std::vector<Trap> traps;
    Trap trap1(200, 0, 80, 80, game);
    traps.push_back(trap1);
    Trap trap2(100, 180, 80, 90, game);
    traps.push_back(trap2);
    Trap trap3(0, 320, 100, 90, game);
    traps.push_back(trap3);
    Trap trap4(280, 570, 300, 110, game);
    traps.push_back(trap4);
    Trap trap5(480, 0, 100, 100, game);
    traps.push_back(trap5);
    Trap trap6(680, 580, 100, 100, game);
    traps.push_back(trap6);
    return traps;
}
