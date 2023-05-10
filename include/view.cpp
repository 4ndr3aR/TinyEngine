#include "view.hpp"

bool View::init(std::string _name, int W, int H, bool headless)
{
  std::cout << "calling View::init..." << std::endl;
  //windowed = true;
  std::cout << "View::windowed: " << windowed << std::endl;
  std::cout << "View::enabled : " << enabled  << std::endl;
  if (headless)
     windowed = false;
  enabled = windowed;
  WIDTH = W; HEIGHT = H;

  #ifndef TINYENGINE_COMPATIBILITY
  if(antialias)
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, antialias);
  #endif

  //Initialize the Window and Context
  Uint32 windowflags = SDL_WINDOW_OPENGL;
  //Uint32 windowflags =SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP;
  //if(!windowed) windowflags = SDL_WINDOW_HIDDEN | SDL_WINDOW_BORDERLESS | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FOREIGN;
  if(!windowed) windowflags |= SDL_WINDOW_HIDDEN;
  //windowflags |= SDL_WINDOW_HIDDEN;

  gWindow = SDL_CreateWindow(_name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, windowflags);
  if( gWindow == NULL )
  {
    printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
    return false;
  }
  SDL_SetWindowResizable(gWindow, SDL_TRUE);

  std::cout << "win: " << windowed << std::endl;
  std::cout << "ena: " << enabled  << std::endl;
  if (windowed || enabled || true)
  {
    std::cout << "maporcozzio: " << windowed << std::endl;
    gContext = SDL_GL_CreateContext(gWindow);
    if( gContext == NULL ){
      printf( "Context could not be created! SDL_Error: %s\n", SDL_GetError() );
      return false;
    }
  }

  SDL_GL_SetSwapInterval(vsync);
  glewExperimental = GL_TRUE;     //Launch GLEW
  glewInit();

  if (windowed || enabled)
  {
    IMGUI_CHECKVERSION();           //Setup ImGUI
    ImGui::CreateContext();
    io = ImGui::GetIO(); (void)io;
    ImGui_ImplSDL2_InitForOpenGL(gWindow, gContext);
    #ifndef TINYENGINE_COMPATIBILITY
    ImGui_ImplOpenGL3_Init("#version 330 core");
    #else
    ImGui_ImplOpenGL3_Init("#version 130");
    #endif
    ImGui::StyleColorsCustom();
  }

  #ifndef TINYENGINE_COMPATIBILITY
  if(antialias)
  glEnable(GL_MULTISAMPLE);
  #endif

  glEnable(GL_DEPTH_TEST);        //Setup Global OpenGL State!
  glDepthFunc(GL_LEQUAL);

  if(blend){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }

  glEnable(GL_CULL_FACE);
  if(ccw) glFrontFace(GL_CCW);
  else glFrontFace(GL_CW);
  glLineWidth(lineWidth);
  glPointSize(pointSize);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  std::cout<<glGetString(GL_VERSION)<<std::endl;

  return true;
}

void View::quit(){
  ImGui_ImplOpenGL3_Shutdown();   //Shutdown ImGUI
  ImGui_ImplSDL2_Shutdown();
  if (windowed || enabled)
  {
    ImGui::DestroyContext();
    SDL_GL_DeleteContext( gContext );
  }
  SDL_DestroyWindow( gWindow );
}

void View::render(){
  (pipeline)();                   //User-Defined Pipeline call

  if(showInterface)               //Draw the interface on top
    drawInterface();

  SDL_GL_SwapWindow(gWindow); //Update Window
}

void View::drawInterface(){
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame(gWindow);
  ImGui::NewFrame();

  (interface)();  //Draw user-defined interface
  //ImGui::ShowDemoWindow();  //Demo-Window (if you want)

  ImGui::Render();
  glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void View::target(glm::vec3 clearcolor, bool clearc, bool cleard){
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, WIDTH, HEIGHT);
  glClearColor(clearcolor.x, clearcolor.y, clearcolor.z, 1.0f);
  if(clearc) glClear(GL_COLOR_BUFFER_BIT);
  if(cleard) glClear(GL_DEPTH_BUFFER_BIT);
}
