#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../utils.h"
#include "../ffmpeg-utils.h"

// Compile with: g++ -std=c++17 -O0 -g3 -DBOOST_LOG_DYN_LINK ../utils.o ../ffmpeg-utils.o minimal.o -Wfatal-errors -lpthread -lSDL2 -lSDL2_image -lGLEW -lboost_system -lboost_log -lboost_filesystem -lX11 -lGL -o minimal

int main(int argc, char* argv[])
{
    int xres = 640;
    int yres = 480;

    init_logging();

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Texture Loading Example",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, xres, yres, 0);
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


    int ffmpeg_w = xres;
    int ffmpeg_h = yres;

    std::string video_fn_out = "/tmp/output-" + curr_date_time() + ".mp4";

    // Create a vector to store the frames
    std::vector<unsigned char> pixels(ffmpeg_w * ffmpeg_h * 3); 

    // Create a pipe for ffmpeg
    auto pipe = ffmpeg_create_pipe(ffmpeg_w, ffmpeg_h, video_fn_out);


    // Wait for user to quit program
    bool quit = false;
    SDL_Event event;
    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }
        }

	capture_gl_image(pixels, ffmpeg_w, ffmpeg_h);
	write_frame_to_ffmpeg(pixels, ffmpeg_w, ffmpeg_h, pipe);
    }

    // Cleanup
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

