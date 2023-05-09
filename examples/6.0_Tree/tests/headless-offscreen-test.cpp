#include <iostream>
//#include <string>
#include <sstream>
//#include <fstream>

#include <stdio.h>
#include <unistd.h>

#include <GL/glew.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_opengl.h>


/*
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
*/

//#include <GL/glext.h>

/*
PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffers = (PFNGLGENFRAMEBUFFERSEXTPROC)glXGetProcAddress((const GLubyte*)"glGenFramebuffersEXT");
PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebuffer = (PFNGLBINDFRAMEBUFFEREXTPROC)glXGetProcAddress((const GLubyte*)"glBindFramebufferEXT");
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)glXGetProcAddress((const GLubyte*)"glFramebufferTexture2DEXT");
*/

/*
struct stb_image_t
{
	unsigned char* data;
	int width, height;	// Image dimensions
	int channels;		// Number of color channels
};
*/

void control_loop(bool& running)
{
    // Handle input events
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

void use_the_texture_luke(GLuint texture)
{
// Create a quad to render the texture to
GLfloat vertices[] = {
    // Position    // Texture coordinates
    -1.0f, -1.0f,  0.0f,  0.0f,  // Bottom-left
     1.0f, -1.0f,  1.0f,  0.0f,  // Bottom-right
     1.0f,  1.0f,  1.0f,  1.0f,  // Top-right
    -1.0f,  1.0f,  0.0f,  1.0f,  // Top-left
};
GLuint indices[] = {
    0, 1, 2,
    0, 2, 3,
};
GLuint vao, vbo, ebo;
glGenVertexArrays(1, &vao);
glGenBuffers(1, &vbo);
glGenBuffers(1, &ebo);
glBindVertexArray(vao);
glBindBuffer(GL_ARRAY_BUFFER, vbo);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
glEnableVertexAttribArray(0);
glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
glEnableVertexAttribArray(1);
//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
glBindVertexArray(0);


// Load and compile a simple vertex and fragment shader
const char* vertex_shader_src =
    "#version 330 core\n"
    "layout (location = 0) in vec2 position;\n"
    "layout (location = 1) in vec2 texCoord;\n"
    "out vec2 TexCoord;\n"
    "void main() {\n"
    "    gl_Position = vec4(position, 0.0, 1.0);\n"
    "    TexCoord = texCoord;\n"
    "}\n";
const char* fragment_shader_src =
    "#version 330 core\n"
    "in vec2 TexCoord;\n"
    "out vec4 color;\n"
    "uniform sampler2D tex;\n"
    "void main() {\n"
    "    color = texture(tex, TexCoord);\n"
    "}\n";
GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
glShaderSource(vertex_shader, 1, &vertex_shader_src, NULL);
glCompileShader(vertex_shader);
GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
glShaderSource(fragment_shader, 1, &fragment_shader_src, NULL);
glCompileShader(fragment_shader);

// Create a shader program
GLuint shader_program = glCreateProgram();
glAttachShader(shader_program, vertex_shader);
glAttachShader(shader_program, fragment_shader);
glLinkProgram(shader_program);

// Get the uniform location for the texture sampler
GLint tex_loc = glGetUniformLocation(shader_program, "tex");

// Use the shader program
glUseProgram(shader_program);

// Bind the texture
glBindTexture(GL_TEXTURE_2D, texture);

// Set the texture sampler uniform
glUniform1i(tex_loc, 0);

// Bind the vertex
}


/*
stb_image_t load_stb_image(std::string fn)
{
    // Load the image using stb_image
    int width, height, channels;
    unsigned char* data = stbi_load(fn.c_str(), &width, &height, &channels, 0);

    stb_image_t image = { data, width, height, channels };

    if (data == NULL)
    {
	// Handle error
	image.data = NULL;
	image.width = -1;
	image.height = -1;
	image.channels = -1;
        fprintf(stderr, "Error loading image: %s\n", stbi_failure_reason());
        return image;
    }

    return image;
}
*/

std::string to_0x_hex_str(int i)
{
    std::stringstream ss;
    ss << "0x" << std::hex << i;
    return ss.str();
}

int main(int argc, char** argv)
{
    bool headless = true;
    int xres = 1920;
    int yres = 1080;

    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = NULL;

    if (headless)
	// Create a hidden window
	window = SDL_CreateWindow("Headless", 0, 0, xres, yres, SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL);
    else
	window = SDL_CreateWindow("Headless", 0, 0, xres, yres, SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL);

/*

    // Create an OpenGL context
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GLContext context = SDL_GL_CreateContext(window);

    // Initialize GLEW
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "Error initializing GLEW: %s\n", glewGetErrorString(err));
        return 1;
    }

    // Check for framebuffer object support
    if (!glewIsSupported("GL_EXT_framebuffer_object")) {
        fprintf(stderr, "Framebuffer object extension not supported\n");
        return 1;
    }

    // Check for a valid OpenGL context
    if (SDL_GL_GetCurrentContext() == NULL) {
        fprintf(stderr, "No current OpenGL context\n");
        return 1;
    }

    // Generate a framebuffer object
    GLuint fbo = 0;
    glGenFramebuffers(1, &fbo);
    if (fbo == 0) {
        fprintf(stderr, "Error generating framebuffer object\n");
        return 1;
    }

    // Bind the framebuffer object
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);



    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version  : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "Vendor        : " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer      : " << glGetString(GL_RENDERER) << std::endl;
std::cout << "-------------" << std::endl;
std::cout << "-------------" << std::endl;
std::cout << "-------------" << std::endl;
std::cout << "-------------" << std::endl;
std::cout << "-------------" << std::endl;
std::cout << "-------------" << std::endl;
std::cout << "-------------" << std::endl;
    /// std::cout << "Extensions    : " << glGetString(GL_EXTENSIONS) << std::endl;
std::cout << "++++++++++++++" << std::endl;
std::cout << "++++++++++++++" << std::endl;
std::cout << "++++++++++++++" << std::endl;
std::cout << "++++++++++++++" << std::endl;
std::cout << "++++++++++++++" << std::endl;
    std::cout << "SDL Context   : " << SDL_GL_GetCurrentContext() << std::endl;
    std::cout << "SDL Window    : " << SDL_GetWindowTitle(window) << std::endl;
    SDL_version buffer;
    SDL_GetVersion(&buffer);
    std::cout << "SDL Version   : " << std::string((char*)(&buffer)) << std::endl;
    for (int i=0 ; i<100 ; i++) std::cout << ".";
    std::cout << std::endl;

    // Create a framebuffer object
    //GLuint fbo;
    //glGenFramebuffers(1, &fbo);
    //glBindFramebuffer(GL_FRAMEBUFFER, fbo);
*/


    //stb_image_t image = load_stb_image("pattern.png");
/*
for (int i=0 ; i<1000 ; i++)
{
    std::cout << to_0x_hex_str(image.data[i]) << " ";
}
    std::cout << std::endl;
*/

// glEnable(GL_TEXTURE_2D);


/*
    // Create a texture to render to
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);




    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload the image data to the texture
    GLenum format = (image.channels == 3) ? GL_RGB : GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, image.width, image.height, 0, format, GL_UNSIGNED_BYTE, image.data);
*/


/*
	// SDL_Window* window = SDL_CreateWindow("Texture Loading Example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, xres, yres, 0);
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
*/





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


bool running = true;
while (running) 
{
    control_loop(running);


/*
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Framebuffer is not complete\n";
            return 1;
        }

        SDL_GL_SwapWindow(window);
        usleep(50000);
*/


/*
// Swap the window buffers
SDL_GL_SwapWindow(window);
    usleep(50000);



    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, xres, yres, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    //uint32_t tex[2][2] = {{16777215,0},{16777215,0}};
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, xres, yres, 0, GL_RGBA, GL_UNSIGNED_BYTE, (const void*)(&tex));
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // Check for framebuffer completeness
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "Framebuffer incomplete: %d\n", status);
        return 1;
    }
*/



    //use_the_texture_luke(texture);

    //usleep(5000000);

}





    // Unbind the texture
    // glBindTexture(GL_TEXTURE_2D, 0);

/*
    // Release the image data
    stbi_image_free(image.data);
*/



    // Render your scene to the texture
    //glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
/*
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
*/
    // ... render your scene ...

/*
    // Read the pixel data from the texture
    unsigned char* pixels = new unsigned char[xres * yres * 4];
    glReadPixels(0, 0, xres, yres, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    // Encode the pixel data with ffmpeg and save to a file
    // ... use ffmpeg to encode the video ...

    // Clean up
    delete[] pixels;

    // glDeleteTextures(1, &texture);
    glDeleteFramebuffers(1, &fbo);
*/

    // Cleanup
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    //SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

