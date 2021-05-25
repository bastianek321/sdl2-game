#include "vectors.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <array>
#include <chrono>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <thread>
#include <tuple>
#include <vector>

void draw_o(std::shared_ptr<SDL_Renderer> r, std::array<double, 2> p, std::shared_ptr<SDL_Texture> tex, double w, double h, double a)
{
    SDL_Rect dst_rect = {(int)(p[0] - w / 2), (int)(p[1] - h / 2), (int)w, (int)h};
    SDL_RenderCopyEx(r.get(), tex.get(), NULL, &dst_rect, a, NULL, SDL_RendererFlip::SDL_FLIP_NONE);
}

class physical_c
{
public:
    std::array<double, 2> position;
    std::array<double, 2> velocity;
    std::array<double, 2> acceleration;

    void update(double dt_f, std::function<void(physical_c *, std::array<double, 2> &pos, std::array<double, 2> &vel)> callback_f)
    {
        using namespace tp::operators;
        std::cout << "acc: " << acceleration[0] << "," << acceleration[1] << " ||| " << velocity[0] << "," << velocity[1] << std::endl;
        auto new_position = position + velocity * dt_f + velocity * acceleration * dt_f * dt_f * 0.5;
        auto new_velocity = (velocity + acceleration * dt_f) * 0.94;
        callback_f(this, new_position, new_velocity);
    }
};

class player_c : public physical_c
{
public:
    std::map<std::string, int> intentions;

    player_c()
    {
        position = {0, 80};
        velocity = {50, -4};
    }

    /**
 * applies and clears intentions
 * */
    void apply_intent()
    {
        acceleration = {0, 50};
        if (intentions.count("left"))
            acceleration[0] += -100;
        if (intentions.count("right"))
            acceleration[0] += 100;

        intentions.clear();
    }
};

int main(int, char **)
{
    using namespace std;
    using namespace std::chrono;
    using namespace tp::operators;

    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);
    shared_ptr<SDL_Window> window_p(
        SDL_CreateWindow("Better Worms", SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED, 640, 360, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE),
        [](auto *window) { SDL_DestroyWindow(window); });

    shared_ptr<SDL_Renderer> renderer_p(
        SDL_CreateRenderer(&*window_p, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC),
        [](auto *renderer) {
            SDL_DestroyRenderer(renderer);
        });

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    SDL_RenderSetLogicalSize(&*renderer_p, 320, 180);

    shared_ptr<SDL_Texture> tex_p(IMG_LoadTexture(&*renderer_p, "data/player.png"),
                                  [](auto *tex) { SDL_DestroyTexture(tex); });

    player_c player;

    milliseconds dt(15);
    steady_clock::time_point current_time = steady_clock::now(); // remember current time

    int mouse_x = 0;
    int mouse_x2 = 0;
    int mouse_y = 0;
    int mouse_y2 = 0;
    int clicks = 0;
    bool render = false;

    for (bool game_active = true; game_active;)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        { // check if there are some events
            if (event.type == SDL_QUIT)
                game_active = false;
            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                if (clicks == 0)
                {
                    mouse_x = event.button.x;
                    mouse_y = event.button.y;
                    clicks += 1;
                }
                else if (clicks == 1)
                {
                    mouse_x2 = event.button.x;
                    mouse_y2 = event.button.y;
                    clicks += 1;
                    render = true;
                }
                else if (clicks == 2){
                    clicks = 0;
                    render = false;
                }
            }
        }
        auto kbdstate = SDL_GetKeyboardState(NULL);
        if (kbdstate[SDL_SCANCODE_LEFT])
            player.intentions["left"] = 1;
        if (kbdstate[SDL_SCANCODE_RIGHT])
            player.intentions["right"] = 1;
        //for (auto [k,v] : player.intentions) {
        //    std::cout << "[" << k << ":" << v << "] ";
        //}
        //std::cout << std::endl;
        //    if (kbdstate[SDL_SCANCODE_RIGHT])

        /// fizyka
        // double dt_f = dt.count() / 1000.0;
        // player.apply_intent();
        // player.update(dt_f, [&](auto p, auto pos, auto vel){
        //     if (pos[1] < 150) {
        //         p->position = pos;
        //         p->velocity = vel;
        //     } else {
        //         p->velocity = {vel[0],0};
        //         p->position[0] = pos[0];
        //     }
        // });
        /// grafika
        SDL_SetRenderDrawColor(&*renderer_p, 0, 100, 20, 255);
        SDL_RenderClear(&*renderer_p);
        SDL_SetRenderDrawColor(&*renderer_p, 255, 100, 200, 255);
        // SDL_RenderCopy(&*renderer_p, &*tex_p, NULL, NULL);
        // draw_o(renderer_p, player.position, tex_p, 16, 16, 30);
        //draw_o(&*renderer_p,{50,20},&*tex_p,16,16,30);
        SDL_Rect rect;
        rect.x = mouse_x;
        rect.y = mouse_y;
        rect.h = mouse_y2 - mouse_y;
        rect.w = mouse_x2 - mouse_x;
        if (render)
        {
            SDL_RenderDrawRect(&*renderer_p, &rect);
        }
        SDL_RenderPresent(&*renderer_p);
        SDL_RenderClear(&*renderer_p);
        this_thread::sleep_until(current_time = current_time + dt);
    }
    SDL_Quit();
    return 0;
}