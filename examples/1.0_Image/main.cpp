#include <TinyEngine/TinyEngine>

#include <TinyEngine/image>
#include <TinyEngine/color>

int main( int argc, char* args[] ) {

        bool headless = false;

	Tiny::window("Shader Effects Example", 1200, 800, headless);

	/*
        if (headless)
                // Create a hidden window
                window = SDL_CreateWindow ("Headless", 0, 0, xres, yres, SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL);
        else
                window = SDL_CreateWindow ("Headless", 0, 0, xres, yres, SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL);
	*/


	Tiny::event.handler = [](){ /* ... */ };

	int ind = 0;    //Effect Index
	int res = 100;  //Pixelate Effect
	int bits = 4;   //Bitreduce Effect

	Tiny::view.interface = [&](){
	  ImGui::Text("Shader Effects");
	  ImGui::DragInt("Effect", &ind, 1, 0, 2);
	  ImGui::DragInt("Resolution", &res, 1, 1, 400);
	  ImGui::DragInt("Bits", &bits, 1, 1, 16);
	};

	Texture tex(image::load("canyon.png"));		//Load Texture with Image
	Square2D flat;														//Create Primitive Model
	Shader effect({"shader/effect.vs", "shader/effect.fs"}, {"in_Quad", "in_Tex"});

	std::string test = "imageTexture";

	Tiny::view.pipeline = [&](){

		Tiny::view.target(color::black);				//Target Main Screen

		effect.use();														//Use Effect Shader
		effect.uniform("index", ind);						//Define Uniforms
		effect.uniform("res", res);
		effect.uniform("bits", bits);
		effect.texture("imageTexture", tex);		//Load Texture
		effect.uniform("model", flat.model);		//Add Model Matrix
		flat.render();													//Render Primitive

	};

	Tiny::loop([&](){
		/* Absolutely Nothing! */
		std::cout << "." << std::flush;
		sleep(1);
	});

	Tiny::quit();

	return 0;
}
