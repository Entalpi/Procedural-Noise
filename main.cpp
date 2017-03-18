#include <iostream>
#include "noise.hpp"
#include <SDL2/SDL.h>

const int WIDTH = 1000;
const int HEIGHT = 1000;
const int ZOOM = 1;
const int DIVISOR = 2;
const int SEED = 1;
const double TIME_STEP = 0.01;

void draw(double time, SDL_Renderer *renderer, Noise &noise_gen) {
    for (size_t x = 0; x < WIDTH; x += ZOOM) {
        for (size_t y = 0; y < HEIGHT; y += ZOOM) {
            // auto noise = noise_gen.octaves(x / DIVISOR, y / DIVISOR, 8, 0.5);
            auto noise = noise_gen.get_value(x / DIVISOR, y / DIVISOR, time / DIVISOR);
            auto color = 125.5f + noise * 125.5f;
            // SDL_Log("Noise: %f, Color: %f \n", noise, color);
            SDL_SetRenderDrawColor(renderer, color, color, color, 1.0f);
            SDL_RenderDrawPoint(renderer, x, y);
        }
    }
    SDL_RenderPresent(renderer);
}

int main() {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);
    Perlin noise(SEED);
    bool quit = false;
    double time = 0.0;
    SDL_Event event;
    while (!quit) {
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                quit = true;
        }
        time += TIME_STEP;
        draw(time, renderer, noise);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}