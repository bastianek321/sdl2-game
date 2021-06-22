#include "game.hpp"


/* INITIALIZES WINDOW */
bool Game::initialize()
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
SDL_Surface *Game::loadImage(std::string path)
{
    SDL_Surface *image = IMG_Load(path.c_str());
    if (image == NULL)
    {
        printf("ERROR DURING LOADING IMAGE");
    }
    return image;
}

SDL_Texture *Game::loadTexture(std::string path)
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
bool Game::loadMedia()
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
void Game::cleanup()
{
    //Destroy window
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    // quit sdl
    // IMG_QUIT();
    SDL_Quit();
}
