#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// Compile with: g++ -std=c++17 -O0 -g3 minimal.cpp -Wfatal-errors -lpthread -lSDL2 -lSDL2_image -lGLEW -lX11 -lGL -o minimal

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Texture Loading Example",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    // Load PNG file as surface
    SDL_Surface* surface = IMG_Load("pattern.png");

    // Convert surface to texture
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    // Free surface
    SDL_FreeSurface(surface);

    // Render texture
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    // Wait for user to quit program
    bool quit = false;
    SDL_Event event;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }
    }

    // Cleanup
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

