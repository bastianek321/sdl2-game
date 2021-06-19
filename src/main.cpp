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

/* ABOVE ARE DONE AND WORKING GREAT ABOVE ARE DONE AND WORKING GREAT ABOVE ARE DONE AND WORKING GREAT */
/* ABOVE ARE DONE AND WORKING GREAT ABOVE ARE DONE AND WORKING GREAT ABOVE ARE DONE AND WORKING GREAT */
/* ABOVE ARE DONE AND WORKING GREAT ABOVE ARE DONE AND WORKING GREAT ABOVE ARE DONE AND WORKING GREAT */
/* ABOVE ARE DONE AND WORKING GREAT ABOVE ARE DONE AND WORKING GREAT ABOVE ARE DONE AND WORKING GREAT */
/* ABOVE ARE DONE AND WORKING GREAT ABOVE ARE DONE AND WORKING GREAT ABOVE ARE DONE AND WORKING GREAT */
/* ABOVE ARE DONE AND WORKING GREAT ABOVE ARE DONE AND WORKING GREAT ABOVE ARE DONE AND WORKING GREAT */

class Object
{
public:
    int x, y, w, h;
    SDL_Texture *texture = loadTexture("data/wall.jpg");
    SDL_Rect obstacle_rect;
    Object()
    {
    }
    Object(int i_x, int i_y, int i_w, int i_h)
    {
        x = i_x;
        y = i_y;
        w = i_w;
        h = i_h;
        obstacle_rect.x = x;
        obstacle_rect.y = y;
        obstacle_rect.w = w;
        obstacle_rect.h = h;
    }

    void draw()
    {

        SDL_RenderDrawRect(renderer, &obstacle_rect);
        SDL_RenderCopy(renderer, texture, NULL, &obstacle_rect);
    }
};

class Player : public Object
{
public:
    int old_x, old_y;
    int hp;
    SDL_Rect player_rect;
    SDL_Texture *texture = loadTexture("data/player.jpg");
    Player(int i_x, int i_y, int i_w, int i_h, int i_hp)
        : Object(i_x, i_y, i_w, i_h)
    {
        hp = i_hp;
        old_x = i_x;
        old_y = i_y;
    }

    void draw()
    {
        player_rect.x = x;
        player_rect.y = y;
        player_rect.w = w;
        player_rect.h = h;
        SDL_RenderDrawRect(renderer, &player_rect);
        SDL_RenderCopy(renderer, texture, NULL, &player_rect);
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
    for (Object obstacle : obstacles)
    {
        if (SDL_HasIntersection(&player.player_rect, &obstacle.obstacle_rect))
        {
            std::cout << "jest" << std::endl;
            collision = true;
            return collision;
        }
        std::cout << "przerwa" << std::endl;
    }
    return collision;
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

    Player player(100, 100, 50, 50, 150);
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
                    player.old_y = player.y;
                    player.y -= 5;
                    break;
                case SDLK_DOWN:
                    player.old_y = player.y;
                    player.y += 5;
                    break;
                case SDLK_LEFT:
                    player.old_x = player.x;
                    player.x -= 5;
                    break;
                case SDLK_RIGHT:
                    player.old_x = player.x;
                    player.x += 5;
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
            player.x = player.old_x;
            player.y = player.old_y;
        }
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