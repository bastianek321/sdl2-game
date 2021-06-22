#ifndef ___COLLISIONS___
#define ___COLLISIONS___

#include "game.hpp"
#include "classes.hpp"
class Game;
class Object;
class Player;
class Trap;

bool check_obstacle_collision(Player player, std::vector<Object> obstacles, Game game);
bool check_player_left_collision(Player player, std::vector<Object> obstacles, Game game);
bool check_player_right_collision(Player player, std::vector<Object> obstacles, Game game);
bool check_player_top_collision(Player player, std::vector<Object> obstacles, Game game);
bool check_player_bottom_collision(Player player, std::vector<Object> obstacles, Game game);
bool check_player_trap_collision(Player player, std::vector<Trap> traps, Game game);
bool check_player_win(Player player, Game game);

#endif