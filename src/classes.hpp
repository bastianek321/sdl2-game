#ifndef ___OBJECT_PLAYER_TRAP_CLASSES___
#define ___OBJECT_PLAYER_TRAP_CLASSES___

#include "game.hpp"
#include "collision.hpp"

class Object
{
public:
    std::array<double, 2> position;
    std::array<double, 2> size;

    SDL_Texture *texture;
    SDL_Rect rect;
    Object();
    Object(double i_x, double i_y, double i_w, double i_h, Game game);

    void draw(Game game);
};

class Player : public Object
{
public:
    std::array<double, 2> old_position;
    std::array<double, 2> velocity;
    std::array<double, 2> acceleration;
    std::string move;
    double friction;
    int hp;
    SDL_Rect hp_bar;

    Player(double i_x, double i_y, double i_w, double i_h, Game game);
        

    void apply_intent();

    void update_hp_bar(double new_hp);

    void update(double dt_f, std::vector<Object> obstacles, Game game);

    void p_draw(Game game);
};

class Trap : public Object
{
public:
    int dmg;
    Trap(int i_x, int i_y, int i_w, int i_h, Game game);
        

    void t_draw(Game game);
};

#endif