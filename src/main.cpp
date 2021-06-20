#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

/* GLOBAL VARIABLES */

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
    std::array<double, 2> velocity;
    std::array<double, 2> acceleration;
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
        rect.x = i_x;
        rect.y = i_y;
        rect.w = i_w;
        rect.h = i_h;
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
    int hp;
    // SDL_Rect hp_bar;
    Player(double i_x, double i_y, double i_w, double i_h, double i_hp)
        : Object(i_x, i_y, i_w, i_h)
    {
        texture = loadTexture("data/player.jpg");
        hp = i_hp;
        old_position.at(0) = i_x;
        old_position.at(1) = i_y;
    }

    void draw()
    {
        rect.x = position.at(0);
        rect.y = position.at(1);
        rect.w = size.at(0);
        rect.h = size.at(1);
        SDL_SetRenderDrawColor(renderer, 0XFF, 0X00, 0X00, 1);
        SDL_RenderDrawLine(renderer, position.at(0), position.at(1) - 15, position.at(0) + hp, position.at(1) - 15);
        // SDL_RenderDrawRect(renderer, &hp_bar);
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

bool check_obstacle_collision(Player player, std::vector<Object> obstacles)
{
    bool collision = false;
    if(player.position.at(0) == 0 || player.position.at(0) + player.size.at(0) == WIDTH || player.position.at(1) == 0 || player.position.at(1) + player.size.at(1) == HEIGHT){
        collision = true;
        return collision;
    }

    for (Object obstacle : obstacles)
    {
        if (SDL_HasIntersection(&player.rect, &obstacle.rect))
        {
            collision = true;
            return collision;
        }
    }
    return collision;
};

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

    Player player(100, 100, 50, 50, 50);
    std::vector<Object> obstacles = generate_obstacles();

    while (running)
    {
        //todo

        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                running = false;
            }
            else if (e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_UP:
                    player.old_position.at(1) = player.position.at(1);
                    player.position.at(1) -= 5;
                    break;
                case SDLK_DOWN:
                    player.old_position.at(1) = player.position.at(1);
                    player.position.at(1) += 5;
                    break;
                case SDLK_LEFT:
                    player.old_position.at(0) = player.position.at(0);
                    player.position.at(0) -= 5;
                    break;
                case SDLK_RIGHT:
                    player.old_position.at(0) = player.position.at(0);
                    player.position.at(0) += 5;
                    break;
                case SDLK_j:
                    player.hp -= 5;
                    break;
                }
            }
        }
        // clears the renderer
        SDL_RenderClear(renderer);

        // copies the texture to the whole window, hence two NULLS
        SDL_RenderCopy(renderer, background, NULL, NULL);

        SDL_SetRenderDrawColor(renderer, 0X88, 0XAF, 0XCF, 0X0F);

        // draws rectangle defined earlier
        if(check_obstacle_collision(player, obstacles)){
            player.position.at(0) = player.old_position.at(0);
            player.position.at(1) = player.old_position.at(1);
        }
        if(player.hp <= 0) running = false;
        draw_obstacles(obstacles);
        player.draw();

        // updates the screen
        SDL_RenderPresent(renderer);

        //blitsurface
        //update the surface
        // SDL_UpdateWindowSurface(window);
    }

    cleanup();

    return 0;
}