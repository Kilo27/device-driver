#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <unistd.h>

// Helper: draws a string at position x, y
void draw_text(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y, SDL_Color color) {
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    int w, h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    SDL_Rect dst = {x, y, w, h};

    SDL_RenderCopy(renderer, texture, NULL, &dst);

    // Clean up — must do this every frame for dynamic text
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

int get_distance() {
    static int d = 0;
    d = (d + 1) % 100;
    return d;
}

int get_volume() {
    static int v = 50;
    v = (v + 1) % 100;
    return v;
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();  // <-- initialise SDL_ttf

    SDL_Window *window = SDL_CreateWindow(
        "Hand Volume Monitor",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        600, 400, 0
    );
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Load a font — change path to a .ttf file on your system
    TTF_Font *font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 24);
    if (!font) {
        printf("Failed to load font: %s\n", TTF_GetError());
        return 1;
    }

    SDL_Color white = {255, 255, 255, 255};

    int running = 1;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event))
            if (event.type == SDL_QUIT) running = 0;

        int distance = get_distance();
        int volume   = get_volume();

        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        // Distance bar + label
        SDL_Rect distance_bar = {50, 100, distance * 4, 40};
        SDL_SetRenderDrawColor(renderer, 0, 200, 255, 255);
        SDL_RenderFillRect(renderer, &distance_bar);

        char dist_label[32];
        snprintf(dist_label, sizeof(dist_label), "Distance: %d", distance);
        draw_text(renderer, font, dist_label, 50, 70, white);

        // Volume bar + label
        SDL_Rect volume_bar = {50, 200, volume * 4, 40};
        SDL_SetRenderDrawColor(renderer, 0, 255, 100, 255);
        SDL_RenderFillRect(renderer, &volume_bar);

        char vol_label[32];
        snprintf(vol_label, sizeof(vol_label), "Volume: %d", volume);
        draw_text(renderer, font, vol_label, 50, 170, white);

        SDL_RenderPresent(renderer);
        SDL_Delay(100);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();  // <-- quit SDL_ttf
    SDL_Quit();
    return 0;
}

/*
```

---

## The Font Path Problem

The trickiest bit is finding a `.ttf` file. Some options:

**Linux** — DejaVu is almost always present:
```
/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf
```

**macOS:**
```
/Library/Fonts/Arial.ttf

*/