#include "vectors.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <chrono>

/* GLOBAL VARIABLES */
class Player;
class Object;
bool check_obstacle_collision(Player player, std::vector<Object> obstacles);
bool check_player_left_collision(Player player, std::vector<Object> obstacles);
bool check_player_right_collision(Player player, std::vector<Object> obstacles);
bool check_player_top_collision(Player player, std::vector<Object> obstacles);
bool check_player_bottom_collision(Player player, std::vector<Object> obstacles);

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
        friction = 0.3;
        rect.w = size.at(0);
        rect.h = size.at(1);
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
        auto new_acceleration = acceleration - velocity * length(velocity);
        auto new_velocity = velocity + new_acceleration * dt_f;
        auto new_position = position + new_velocity * dt_f + new_acceleration * dt_f * dt_f * 0.5;
        old_position = position;
        velocity = new_velocity;
        if(velocity.at(0) < 0 && check_player_left_collision(*this, obstacles)){
            velocity.at(0) = 0;
            new_position.at(0) = position.at(0);
        }
        if(velocity.at(0) > 0 && check_player_right_collision(*this, obstacles)){
            velocity.at(0) = 0;
            new_position.at(0) = position.at(0);
        }
        if(velocity.at(1) < 0 && check_player_top_collision(*this, obstacles)){
            velocity.at(1) = 0;
            new_position.at(1) = position.at(1);
        }
        if(velocity.at(1) > 0 && check_player_bottom_collision(*this, obstacles)){
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

std::vector<Object> generate_obstacles()
{
    std::vector<Object> obstacles;
    Object obstacle(200, 50, 200, 200);
    obstacles.push_back(obstacle);
    Object obstacle2(450, 300, 100, 150);
    obstacles.push_back(obstacle2);

    return obstacles;
}
void draw_obstacles(std::vector<Object> obstacles)
{
    for (Object obstacle : obstacles)
    {
        obstacle.draw();
    }
}

bool check_player_left_collision(Player player, std::vector<Object> obstacles){
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

bool check_player_right_collision(Player player, std::vector<Object> obstacles){
    SDL_Rect right_rect;
    right_rect.x = player.position.at(0) + 5;
    right_rect.y = player.position.at(1);
    right_rect.w = player.size.at(0);
    right_rect.h = player.size.at(1);

    if(right_rect.x + right_rect.w >= WIDTH){
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

bool check_player_top_collision(Player player, std::vector<Object> obstacles){
    SDL_Rect top_rect;
    top_rect.x = player.position.at(0);
    top_rect.y = player.position.at(1) - 5;
    top_rect.w = player.size.at(0);
    top_rect.h = player.size.at(1);

    if(top_rect.y == 0){
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

bool check_player_bottom_collision(Player player, std::vector<Object> obstacles){
    SDL_Rect bottom_rect;
    bottom_rect.x = player.position.at(0);
    bottom_rect.y = player.position.at(1) + 5;
    bottom_rect.w = player.size.at(0);
    bottom_rect.h = player.size.at(1);

    if(bottom_rect.y + bottom_rect.h >= HEIGHT){
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


/* ABOVE ARE DONE AND WORKING GREAT ABOVE ARE DONE AND WORKING GREAT ABOVE ARE DONE AND WORKING GREAT */
/* ABOVE ARE DONE AND WORKING GREAT ABOVE ARE DONE AND WORKING GREAT ABOVE ARE DONE AND WORKING GREAT */
/* ABOVE ARE DONE AND WORKING GREAT ABOVE ARE DONE AND WORKING GREAT ABOVE ARE DONE AND WORKING GREAT */
/* ABOVE ARE DONE AND WORKING GREAT ABOVE ARE DONE AND WORKING GREAT ABOVE ARE DONE AND WORKING GREAT */
/* ABOVE ARE DONE AND WORKING GREAT ABOVE ARE DONE AND WORKING GREAT ABOVE ARE DONE AND WORKING GREAT */
/* ABOVE ARE DONE AND WORKING GREAT ABOVE ARE DONE AND WORKING GREAT ABOVE ARE DONE AND WORKING GREAT */

class Bullet : public Object
{
public:
    int dmg;
    Bullet(int i_x, int i_y, int i_w, int i_h, int i_dmg)
        : Object(i_x, i_y, i_w, i_h)
    {
        dmg = i_dmg;
    }

    void draw()
    {
        rect.x = position.at(0);
        rect.y = position.at(1);
        rect.w = size.at(0);
        rect.h = size.at(1);
        SDL_SetRenderDrawColor(renderer, 0X0F, 0X5F, 0X3F, 255);
        SDL_RenderFillRect(renderer, &rect);
        SDL_RenderDrawRect(renderer, &rect);
    }
};

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

    Player player(100, 150, 50, 50);
    std::vector<Object> obstacles = generate_obstacles();
    std::chrono::milliseconds timer(15);

    while (running)
    {
        double dt_f = timer.count() / 1000.0;
        // std::cout << player.acceleration.at(0) << std::endl;
        // std::cout << player.acceleration.at(1) << std::endl;
        // std::cout << std::endl;
        std::cout << player.velocity.at(0) << std::endl;
        std::cout << player.velocity.at(1) + player.size.at(1) << std::endl;
        // std::cout << std::endl;
        player.acceleration = {0, 5};

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
                    player.acceleration.at(1) -= 3500;
                }
                if (e.key.keysym.sym == SDLK_DOWN)
                {
                    player.acceleration.at(1) += 500;
                }
                if (e.key.keysym.sym == SDLK_LEFT)
                {
                    player.acceleration.at(0) -= 500;
                }
                if (e.key.keysym.sym == SDLK_RIGHT)
                {
                    player.acceleration.at(0) += 500;
                }
                if (e.key.keysym.sym == SDLK_j)
                {
                    player.hp -= 5;
                }
                if(e.key.keysym.sym == SDLK_SPACE){
                    player.velocity.at(1) -= 10;
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
            running = false;
        draw_obstacles(obstacles);
        player.update(dt_f, obstacles);
        // if (check_obstacle_collision(player, obstacles))
        // {
        //     player.position.at(0) = player.old_position.at(0);
        //     player.position.at(1) = player.old_position.at(1);
        // }
        player.p_draw();

        // updates the screen
        SDL_RenderPresent(renderer);
    }

    cleanup();

    return 0;
}