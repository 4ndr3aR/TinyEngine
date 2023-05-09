#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

struct stb_image_t
{
    unsigned char* data;
    int width, height;  // Image dimensions
    int channels;       // Number of color channels
};

void control_loop(bool& running)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            running = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                running = false;
            }
            break;
        }
    }
}

stb_image_t load_stb_image(std::string fn)
{
    int width, height, channels;
    unsigned char* data = stbi_load(fn.c_str(), &width, &height, &channels, 0);
    stb_image_t image = { data, width, height, channels };
    if (data == NULL)
    {
        image.data = NULL;
        image.width = -1;
        image.height = -1;
        image.channels = -1;
        fprintf(stderr, "Error loading image: %s\n", stbi_failure_reason());
        return image;
    }
    return image;
}

int main(int argc, char** argv)
{
    bool headless = false;
    int xres = 1920;
    int yres = 1080;

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = NULL;

    if (headless)
        window = SDL_CreateWindow("Headless", 0, 0, xres, yres, SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL);
    else
        window = SDL_CreateWindow("Headless", 0, 0, xres, yres, SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);

    SDL_GLContext context = SDL_GL_CreateContext(window);

    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        fprintf(stderr, "Error initializing GLEW: %s\n", glewGetErrorString(err));
        return 1;
    }

    if (!glewIsSupported("GL_EXT_framebuffer_object"))
    {
        fprintf(stderr, "Framebuffer object extension not supported\n");
        return 1;
    }

    if (SDL_GL_GetCurrentContext() == NULL)
    {
        fprintf(stderr, "No current OpenGL context\n");
        return 1;
    }

    GLuint fbo = 0;
    glGenFramebuffers(1, &fbo);
    if (fbo == 0)
    {
        fprintf(stderr, "Error generating framebuffer object\n");
        return 1;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    stb_image_t image = load_stb_image("/tmp/test.png");

    glEnable(GL_TEXTURE_2D);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D

