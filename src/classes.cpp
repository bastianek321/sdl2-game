#include "classes.hpp"
#include "game.hpp"

Object::Object()
{
}
Object::Object(double i_x, double i_y, double i_w, double i_h, Game game)
{
    texture = game.loadTexture("data/wall.jpg");
    position.at(0) = i_x;
    position.at(1) = i_y;
    size.at(0) = i_w;
    size.at(1) = i_h;
    rect.x = position.at(0);
    rect.y = position.at(1);
    rect.w = size.at(0);
    rect.h = size.at(1);
}

void Object::draw(Game game)
{
    SDL_RenderDrawRect(game.renderer, &rect);
    SDL_RenderCopy(game.renderer, texture, NULL, &rect);
}

Player::Player(double i_x, double i_y, double i_w, double i_h, Game game)
    : Object(i_x, i_y, i_w, i_h, game)
{
    texture = game.loadTexture("data/player.jpg");
    hp = 50;
    old_position.at(0) = i_x;
    old_position.at(1) = i_y;
    friction = 0.5;
    rect.w = size.at(0);
    rect.h = size.at(1);
}

void Player::apply_intent()
{
    acceleration = {0, 10};
    if (move == "right")
        acceleration[0] += 1000;
    if (move == "left")
        acceleration[0] += -1000;
    if (move == "up")
        acceleration[1] += -4500;
    if (move == "down")
        acceleration[1] += +1000;
    move = " ";
}

void Player::update_hp_bar(double new_hp)
{
    hp_bar.x = position.at(0);
    hp_bar.y = position.at(1) - 15;
    hp_bar.w = new_hp;
    hp_bar.h = 10;
}

void Player::update(double dt_f, std::vector<Object> obstacles, Game game)
{
    using namespace tp::operators;
    // apply friction:
    auto new_acceleration = acceleration - velocity * length(velocity) * friction;
    auto new_velocity = velocity + new_acceleration * dt_f;
    auto new_position = position + new_velocity * dt_f + new_acceleration * dt_f * dt_f * 0.5;
    old_position = position;
    velocity = new_velocity;
    if (velocity.at(0) < 0 && check_player_left_collision(*this, obstacles, game))
    {
        velocity.at(0) = 0;
        new_position.at(0) = position.at(0);
    }
    if (velocity.at(0) > 0 && check_player_right_collision(*this, obstacles, game))
    {
        velocity.at(0) = 0;
        new_position.at(0) = position.at(0);
    }
    if (velocity.at(1) < 0 && check_player_top_collision(*this, obstacles, game))
    {
        velocity.at(1) = 0;
        new_position.at(1) = position.at(1);
    }
    if (velocity.at(1) > 0 && check_player_bottom_collision(*this, obstacles, game))
    {
        velocity.at(1) = 0;
        new_position.at(1) = position.at(1);
    }
    position = new_position;
    acceleration = new_acceleration;
    rect.x = position.at(0);
    rect.y = position.at(1);
}

void Player::p_draw(Game game)
{
    SDL_SetRenderDrawColor(game.renderer, 0XFF, 0X00, 0X00, 1);
    update_hp_bar(hp);
    SDL_RenderFillRect(game.renderer, &hp_bar);
    SDL_RenderDrawRect(game.renderer, &hp_bar);
    SDL_RenderDrawRect(game.renderer, &rect);
    SDL_RenderCopy(game.renderer, texture, NULL, &rect);
}

Trap::Trap(int i_x, int i_y, int i_w, int i_h, Game game)
    : Object(i_x, i_y, i_w, i_h, game)
{
    texture = game.loadTexture("data/trap.png");
}

void Trap::t_draw(Game game)
{
    rect.x = position.at(0);
    rect.y = position.at(1);
    rect.w = size.at(0);
    rect.h = size.at(1);
    SDL_SetRenderDrawColor(game.renderer, 0X0F, 0X5F, 0X3F, 255);
    SDL_RenderDrawRect(game.renderer, &rect);
    SDL_RenderCopy(game.renderer, texture, NULL, &rect);
}