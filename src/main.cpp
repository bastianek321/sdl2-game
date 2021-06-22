#include "vectors.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <map>

/* GLOBAL VARIABLES */
class Object;
class Player;
class Trap;
bool check_obstacle_collision(Player player, std::vector<Object> obstacles);
bool check_player_left_collision(Player player, std::vector<Object> obstacles);
bool check_player_right_collision(Player player, std::vector<Object> obstacles);
bool check_player_top_collision(Player player, std::vector<Object> obstacles);
bool check_player_bottom_collision(Player player, std::vector<Object> obstacles);
bool check_player_trap_collision(Player player, std::vector<Trap> traps);

// screen size
static const int WIDTH = 840;
static const int HEIGHT = 680;

// window that we'll be rendering to
SDL_Window *window = NULL;

// surface contained by that window
SDL_Surface *surface = NULL;

//The image we will load and show on the screen
SDL_Texture *background = NULL;

SDL_Texture *playerTexture = NULL;

//The renderer
SDL_Renderer *renderer;

/* KEYBOARD INPUTS */
enum KeyPress
{
    KEY_PRESS_UP,
    KEY_PRESS_DOWN,
    KEY_PRESS_LEFT,
    KEY_PRESS_RIGHT
};

/* INITIALIZES WINDOW */
bool initialize()
{
    bool success = true;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize ! \n%s\n\n", SDL_GetError());
        success = false;
    }
    else
    {
        // create window
        window = SDL_CreateWindow("Dont know", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
        if (window == NULL)
        {
            /* to show the error SDL_GetError() */
            printf("Window could not be created!");
            success = false;
        }
        else
        {
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

            if (renderer == NULL)
            {
                printf("Renderer could not be created");
                success = false;
            }
            else
            {
                //set basic draw color to white
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            }
        }
    }
    return success;
}

/* LOADS A JPG IMAGE BY PATH */
SDL_Surface *loadImage(std::string path)
{
    SDL_Surface *image = IMG_Load(path.c_str());
    if (image == NULL)
    {
        printf("ERROR DURING LOADING IMAGE");
    }
    return image;
}

SDL_Texture *loadTexture(std::string path)
{
    SDL_Texture *texture = NULL;
    try
    {
        texture = SDL_CreateTextureFromSurface(renderer, loadImage(path));
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    return texture;
}

/* LOADS MEDIA */
bool loadMedia()
{
    bool success = true;
    //get window surface
    surface = SDL_GetWindowSurface(window);

    //fill the surface with white
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0XFF, 0XFF, 0XFF));

    playerTexture = loadTexture("data/player.jpg");
    background = loadTexture("data/ambient.jpg");
    if (background == NULL)
    {
        printf("Image doesnt exist");
        success = false;
    }
    return success;
}

/* FREES RESOURCES */
void cleanup()
{
    //Destroy window
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    // quit sdl
    // IMG_QUIT();
    SDL_Quit();
}

class Object
{
public:
    std::array<double, 2> position;
    std::array<double, 2> size;

    SDL_Texture *texture;
    SDL_Rect rect;
    Object()
    {
    }
    Object(double i_x, double i_y, double i_w, double i_h)
    {
        texture = loadTexture("data/wall.jpg");
        position.at(0) = i_x;
        position.at(1) = i_y;
        size.at(0) = i_w;
        size.at(1) = i_h;
        rect.x = position.at(0);
        rect.y = position.at(1);
        rect.w = size.at(0);
        rect.h = size.at(1);
    }

    void draw()
    {
        SDL_RenderDrawRect(renderer, &rect);
        SDL_RenderCopy(renderer, texture, NULL, &rect);
    }
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

    Player(double i_x, double i_y, double i_w, double i_h)
        : Object(i_x, i_y, i_w, i_h)
    {
        texture = loadTexture("data/player.jpg");
        hp = 50;
        old_position.at(0) = i_x;
        old_position.at(1) = i_y;
        friction = 0.5;
        rect.w = size.at(0);
        rect.h = size.at(1);
    }

    void apply_intent()
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

    void update_hp_bar(double new_hp)
    {
        hp_bar.x = position.at(0);
        hp_bar.y = position.at(1) - 15;
        hp_bar.w = new_hp;
        hp_bar.h = 10;
    }

    void update(double dt_f, std::vector<Object> obstacles)
    {
        using namespace tp::operators;
        // apply friction:
        auto new_acceleration = acceleration - velocity * length(velocity) * friction;
        auto new_velocity = velocity + new_acceleration * dt_f;
        auto new_position = position + new_velocity * dt_f + new_acceleration * dt_f * dt_f * 0.5;
        old_position = position;
        velocity = new_velocity;
        if (velocity.at(0) < 0 && check_player_left_collision(*this, obstacles))
        {
            velocity.at(0) = 0;
            new_position.at(0) = position.at(0);
        }
        if (velocity.at(0) > 0 && check_player_right_collision(*this, obstacles))
        {
            velocity.at(0) = 0;
            new_position.at(0) = position.at(0);
        }
        if (velocity.at(1) < 0 && check_player_top_collision(*this, obstacles))
        {
            velocity.at(1) = 0;
            new_position.at(1) = position.at(1);
        }
        if (velocity.at(1) > 0 && check_player_bottom_collision(*this, obstacles))
        {
            velocity.at(1) = 0;
            new_position.at(1) = position.at(1);
        }
        position = new_position;
        acceleration = new_acceleration;
        rect.x = position.at(0);
        rect.y = position.at(1);
    }

    void p_draw()
    {
        SDL_SetRenderDrawColor(renderer, 0XFF, 0X00, 0X00, 1);
        update_hp_bar(hp);
        SDL_RenderFillRect(renderer, &hp_bar);
        SDL_RenderDrawRect(renderer, &hp_bar);
        SDL_RenderDrawRect(renderer, &rect);
        SDL_RenderCopy(renderer, texture, NULL, &rect);
    }
};

class Trap : public Object
{
public:
    int dmg;
    Trap(int i_x, int i_y, int i_w, int i_h)
        : Object(i_x, i_y, i_w, i_h)
    {
        texture = loadTexture("data/trap.png");
    }

    void t_draw()
    {
        rect.x = position.at(0);
        rect.y = position.at(1);
        rect.w = size.at(0);
        rect.h = size.at(1);
        SDL_SetRenderDrawColor(renderer, 0X0F, 0X5F, 0X3F, 255);
        SDL_RenderDrawRect(renderer, &rect);
        SDL_RenderCopy(renderer, texture, NULL, &rect);
    }
};

std::vector<Object> generate_obstacles()
{
    std::vector<Object> obstacles;
    Object obstacle(0, 80, 200, 100); // 1
    obstacles.push_back(obstacle);
    Object obstacle2(280, 0, 100, 270); // 2
    obstacles.push_back(obstacle2);
    Object obstacle3(100, 270, 280, 100); // 3
    obstacles.push_back(obstacle3);
    Object obstacle4(100, 370, 100, 200); // 4
    obstacles.push_back(obstacle4);
    Object obstacle5(280, 475, 300, 100); // 5
    obstacles.push_back(obstacle5);
    Object obstacle6(480, 100, 100, 380); // 6
    obstacles.push_back(obstacle6);
    Object obstacle7(680, 0, 100, 580); // 7
    obstacles.push_back(obstacle7);
    return obstacles;
}

std::vector<Trap> generate_traps()
{
    std::vector<Trap> traps;
    Trap trap1(200, 0, 80, 80);
    traps.push_back(trap1);
    Trap trap2(100, 180, 80, 90);
    traps.push_back(trap2);
    Trap trap3(0, 320, 100, 90);
    traps.push_back(trap3);
    Trap trap4(280, 570, 300, 110);
    traps.push_back(trap4);
    Trap trap5(480, 0, 100, 100);
    traps.push_back(trap5);
    Trap trap6(680, 580, 100, 100);
    traps.push_back(trap6);
    return traps;
}

void draw_obstacles(std::vector<Object> obstacles)
{
    for (Object obstacle : obstacles)
    {
        obstacle.draw();
    }
}

void draw_traps(std::vector<Trap> traps)
{
    for (Trap trap : traps)
    {
        trap.t_draw();
    }
}

bool check_player_left_collision(Player player, std::vector<Object> obstacles)
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

bool check_player_right_collision(Player player, std::vector<Object> obstacles)
{
    SDL_Rect right_rect;
    right_rect.x = player.position.at(0) + 5;
    right_rect.y = player.position.at(1);
    right_rect.w = player.size.at(0);
    right_rect.h = player.size.at(1);

    if (right_rect.x + right_rect.w >= WIDTH)
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

bool check_player_top_collision(Player player, std::vector<Object> obstacles)
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

bool check_player_bottom_collision(Player player, std::vector<Object> obstacles)
{
    SDL_Rect bottom_rect;
    bottom_rect.x = player.position.at(0);
    bottom_rect.y = player.position.at(1) + 5;
    bottom_rect.w = player.size.at(0);
    bottom_rect.h = player.size.at(1);

    if (bottom_rect.y + bottom_rect.h >= HEIGHT)
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

bool check_player_trap_collision(Player player, std::vector<Trap> traps)
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

bool check_player_win(Player player)
{
    if (player.position.at(0) > 780 && player.position.at(1) < 10)
    {
        return true;
    }
    return false;
}

void draw_win_tile()
{
    SDL_Rect win_rect;
    SDL_SetRenderDrawColor(renderer, 0X00, 0XFF, 0X00, 100);
    win_rect.x = WIDTH - 60;
    win_rect.y = 0;
    win_rect.h = 60;
    win_rect.w = 60;
    SDL_RenderDrawRect(renderer, &win_rect);
}

/* ABOVE ARE DONE AND WORKING GREAT ABOVE ARE DONE AND WORKING GREAT ABOVE ARE DONE AND WORKING GREAT */
/* ABOVE ARE DONE AND WORKING GREAT ABOVE ARE DONE AND WORKING GREAT ABOVE ARE DONE AND WORKING GREAT */

int main(int, char **)
{

    if (!initialize())
    {
        printf("ERROR DURING INITIALIZATION");
    }
    if (!loadMedia())
    {
        printf("ERROR DURING LOADING MEDIA");
    }

    //main loop variables
    bool running = true;
    SDL_Event e;

    Player player(10, 10, 50, 50);
    std::vector<Object> obstacles = generate_obstacles();
    std::vector<Trap> traps = generate_traps();
    std::vector<Trap> show_traps = traps;
    std::chrono::milliseconds timer(15);
    int iterations = 0;

    bool hit = false;
    int damage_cooldown = 0;

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
        SDL_RenderClear(renderer);

        // copies the texture to the whole window, hence two NULLS
        SDL_RenderCopy(renderer, background, NULL, NULL);

        SDL_SetRenderDrawColor(renderer, 0X88, 0XAF, 0XCF, 0X0F);

        // draws rectangle defined earlier

        if (player.hp <= 0)
        {
            std::cout << "Przegrales" << std::endl;
            running = false;
        }
        if (check_player_win(player))
        {
            std::cout << "Wygrales" << std::endl;
            running = false;
        }
        draw_obstacles(obstacles);
        draw_win_tile();
        player.apply_intent();
        if ((iterations > 2000 && iterations < 4000) || (iterations > 7000 && iterations < 8000))
        {
            show_traps = traps;
            draw_traps(show_traps);
            if (check_player_trap_collision(player, traps))
            {
                if (!hit)
                {
                    hit = true;
                    if (player.position.at(0) > 260 && player.position.at(0) < 580 &&
                        player.position.at(1) > 570)
                    {
                        player.hp -= 30;
                    }
                    player.hp -= 5;
                }
            }
        }
        else
        {
            show_traps.clear();
        }

        player.update(dt_f, obstacles);
        player.p_draw();

        // updates the screen
        SDL_RenderPresent(renderer);
    }

    cleanup();

    return 0;
}