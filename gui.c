#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <unistd.h>

int get_distance() {
    // Later replace with reading from your driver
    static int d = 0;
    d = (d + 1) % 100;
    return d;
}

int get_volume() {
    // Later replace with ioctl or read()
    static int v = 50;
    v = (v + 1) % 100;
    return v;
}

int main() {

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow(
        "Hand Volume Monitor",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        600,
        400,
        0
    );

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    int running = 1;
    SDL_Event event;

    while (running) {

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = 0;
        }

        int distance = get_distance();
        int volume = get_volume();

        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        // Distance bar
        SDL_Color textColor = {0, 0, 0, 255}; // black color
        SDL_Surface *textSurface = TTF_RenderText_Solid(font, "Hello World!", textColor);
        SDL_Rect textRect = {50, 50, textSurface->w, textSurface->h}; // rectangle where the text is drawn 
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_Rect distance_bar = {50, 100, distance * 4, 40};
        SDL_SetRenderDrawColor(renderer, 0, 200, 255, 255);
        SDL_RenderFillRect(renderer, &distance_bar);

        // Volume bar
        SDL_Rect volume_bar = {50, 200, volume * 4, 40};
        SDL_SetRenderDrawColor(renderer, 0, 255, 100, 255);
        SDL_RenderFillRect(renderer, &volume_bar);

        SDL_RenderPresent(renderer);

        SDL_Delay(100);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
