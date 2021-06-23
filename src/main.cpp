#include "generators.hpp"

Game game;

void draw_obstacles(std::vector<Object> obstacles)
{
    for (Object obstacle : obstacles)
    {
        obstacle.draw(game);
    }
}

void draw_traps(std::vector<Trap> traps)
{
    for (Trap trap : traps)
    {
        trap.t_draw(game);
    }
}

void draw_win_tile()
{
    SDL_Rect win_rect;
    SDL_SetRenderDrawColor(game.renderer, 0X00, 0XFF, 0X00, 100);
    win_rect.x = game.WIDTH - 60;
    win_rect.y = 0;
    win_rect.h = 60;
    win_rect.w = 60;
    SDL_RenderDrawRect(game.renderer, &win_rect);
}

int main(int, char **)
{
   

    if (!game.initialize())
    {
        printf("ERROR DURING INITIALIZATION");
    }
    if (!game.loadMedia())
    {
        printf("ERROR DURING LOADING MEDIA");
    }

    //main loop variables
    bool running = true;
    SDL_Event e;

    Player player(10, 10, 50, 50, game);
    std::vector<Object> obstacles = generate_obstacles(game);
    std::vector<Trap> traps = generate_traps(game);
    std::vector<Trap> show_traps = traps;
    std::chrono::milliseconds timer(15);
    int iterations = 0;

    bool hit = false;
    int damage_cooldown = 0;
    SDL_Rect message;
    message.x = 0;
    message.y = 0;
    message.w = game.WIDTH;
    message.h = game.HEIGHT;

    while (running)
    {
        iterations += 1;
        if (iterations >= 10000)
        {
            iterations = 0;
        }
        if (hit)
        {
            damage_cooldown += 1;
            if (damage_cooldown >= 2000)
            {
                damage_cooldown = 0;
                hit = false;
            }
        }

        double dt_f = timer.count() / 1000.0;

        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                running = false;
            }
            else if (e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.sym == SDLK_UP)
                {
                    player.move = "up";
                }
                if (e.key.keysym.sym == SDLK_DOWN)
                {
                    player.move = "down";
                }
                if (e.key.keysym.sym == SDLK_LEFT)
                {
                    player.move = "left";
                }
                if (e.key.keysym.sym == SDLK_RIGHT)
                {
                    player.move = "right";
                }
            }
        }
        // clears the renderer
        SDL_RenderClear(game.renderer);

        // copies the texture to the whole window, hence two NULLS
        SDL_RenderCopy(game.renderer, game.background, NULL, NULL);

        SDL_SetRenderDrawColor(game.renderer, 0X88, 0XAF, 0XCF, 0X0F);

        // draws rectangle defined earlier

        draw_obstacles(obstacles);
        draw_win_tile();
        player.apply_intent();
        if ((iterations > 2000 && iterations < 4000) || (iterations > 7000 && iterations < 8000))
        {
            show_traps = traps;
            draw_traps(show_traps);
            if (check_player_trap_collision(player, traps, game))
            {
                if (!hit)
                {
                    hit = true;
                    if (player.position.at(0) > 260 && player.position.at(0) < 580 &&
                        player.position.at(1) > 570)
                    {
                        player.hp -= 20;
                    }
                    player.hp -= 5;
                }
            }
        }
        else
        {
            show_traps.clear();
        }

        player.update(dt_f, obstacles, game);
        player.p_draw(game);
        if (player.hp <= 0)
        {
            std::cout << "Przegrales" << std::endl;
            SDL_SetRenderDrawColor(game.renderer, 0XAF, 0X00, 0X00, 100);
            SDL_RenderFillRect(game.renderer, &message);
            SDL_RenderDrawRect(game.renderer, &message);
            running = false;
        } else if (check_player_win(player, game))
        {
            std::cout << "Wygrales" << std::endl;
            SDL_SetRenderDrawColor(game.renderer, 0X00, 0XAF, 0X00, 100);
            SDL_RenderFillRect(game.renderer, &message);
            SDL_RenderDrawRect(game.renderer, &message);
            running = false;
        }

        // updates the screen
        SDL_RenderPresent(game.renderer);
    }

    game.cleanup();

    return 0;
}