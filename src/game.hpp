#ifndef ___GAME_CLASS___
#define ___GAME_CLASS___

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "vectors.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <map>

class Game
{
public:
    static const int WIDTH = 840;
    static const int HEIGHT = 680;

    // window that we'll be rendering to
    SDL_Window *window;

    // surface contained by that window
    SDL_Surface *surface;

    //The image we will load and show on the screen
    SDL_Texture *background;

    SDL_Texture *playerTexture;

    //The renderer
    SDL_Renderer *renderer;

    bool initialize();
    SDL_Surface *loadImage(std::string path);
    SDL_Texture *loadTexture(std::string path);
    bool loadMedia();
    void cleanup();

};

#endif