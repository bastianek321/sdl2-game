#include "collision.hpp"

bool check_player_left_collision(Player player, std::vector<Object> obstacles, Game game)
{
    SDL_Rect left_rect;
    left_rect.x = player.position.at(0) - 5;
    left_rect.y = player.position.at(1);
    left_rect.w = player.size.at(0);
    left_rect.h = player.size.at(1);

    if (left_rect.x == 0)
    {
        return true;
    }

    for (Object obstacle : obstacles)
    {
        if (SDL_HasIntersection(&left_rect, &obstacle.rect))
        {
            return true;
        }
    }
    return false;
}

bool check_player_right_collision(Player player, std::vector<Object> obstacles, Game game)
{
    SDL_Rect right_rect;
    right_rect.x = player.position.at(0) + 5;
    right_rect.y = player.position.at(1);
    right_rect.w = player.size.at(0);
    right_rect.h = player.size.at(1);

    if (right_rect.x + right_rect.w >= game.WIDTH)
    {
        return true;
    }

    for (Object obstacle : obstacles)
    {
        if (SDL_HasIntersection(&right_rect, &obstacle.rect))
        {
            return true;
        }
    }
    return false;
}

bool check_player_top_collision(Player player, std::vector<Object> obstacles, Game game)
{
    SDL_Rect top_rect;
    top_rect.x = player.position.at(0);
    top_rect.y = player.position.at(1) - 5;
    top_rect.w = player.size.at(0);
    top_rect.h = player.size.at(1);

    if (top_rect.y == 0)
    {
        return true;
    }

    for (Object obstacle : obstacles)
    {
        if (SDL_HasIntersection(&top_rect, &obstacle.rect))
        {
            return true;
        }
    }
    return false;
}

bool check_player_bottom_collision(Player player, std::vector<Object> obstacles, Game game)
{
    SDL_Rect bottom_rect;
    bottom_rect.x = player.position.at(0);
    bottom_rect.y = player.position.at(1) + 5;
    bottom_rect.w = player.size.at(0);
    bottom_rect.h = player.size.at(1);

    if (bottom_rect.y + bottom_rect.h >= game.HEIGHT)
    {
        return true;
    }

    for (Object obstacle : obstacles)
    {
        if (SDL_HasIntersection(&bottom_rect, &obstacle.rect))
        {
            return true;
        }
    }
    return false;
}

bool check_player_trap_collision(Player player, std::vector<Trap> traps, Game game)
{
    SDL_Rect player_rect;
    player_rect.x = player.position.at(0);
    player_rect.y = player.position.at(1);
    player_rect.w = player.size.at(0);
    player_rect.h = player.size.at(1);

    for (Trap trap : traps)
    {
        if (SDL_HasIntersection(&player_rect, &trap.rect))
        {
            player.hp -= trap.dmg;
            return true;
        }
    }
    return false;
}

bool check_player_win(Player player, Game game)
{
    if (player.position.at(0) > 780 && player.position.at(1) < 10)
    {
        return true;
    }
    return false;
}
