#include <TinyEngine/TinyEngine>
#include <TinyEngine/image>
#include <TinyEngine/color>
#include <TinyEngine/camera>

#define PI 3.14159265f

#include <random>

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

#include "model.h"
#include "utils.h"

#include "ffmpeg-utils.h"

int main( int argc, char* args[] )
{
	bool headless = false;
	int xres = 1920, yres = 1080;

        init_logging();

	asdf();
        std::cout << "asdf" << std::endl;
	Tiny::view.windowed = false;
	Tiny::view.lineWidth = 1.0f;

        std::cout << "qwer" << std::endl;
	Tiny::view.windowed = true;
	Tiny::view.enabled = true;
	Tiny::window("Procedural Tree", xres, yres, headless);
        std::cout << "poiu" << std::endl;
	cam::look = glm::vec3(0, 100, 0);
	cam::far = 2000.0f;
	cam::roty = 25.0f;
	cam::zoomrate = 10.0f;
	cam::init(600, cam::PROJ);

	bool paused = false;
	bool autorotate = true;

	Tiny::event.handler = [&](){
	  (cam::handler)();
	  if(!Tiny::event.press.empty()){
	    if(Tiny::event.press.back() == SDLK_p)
	      paused = !paused;
	    else if(Tiny::event.press.back() == SDLK_a)
	      autorotate = !autorotate;
	    else if(Tiny::event.press.back() == SDLK_r){
	      Branch* newroot = new Branch(root, true);
	      delete(root);
	      root = newroot;
	    }
	  }
	};

	Tiny::view.interface = interfaceFunc;

	root = new Branch({0.6, 0.45, 2.5}); //Create Root

	Buffer positions, normals, colors;
	Buffer indices;
	construct(positions, normals, colors, indices);

	Model treemesh({"in_Position", "in_Normal", "in_Color"});
	treemesh.bind<glm::vec3>("in_Position", &positions);
	treemesh.bind<glm::vec3>("in_Normal", &normals);
	treemesh.bind<glm::vec4>("in_Color", &colors);
	treemesh.index(&indices);

	Square3D flat;														//Geometry for Particle System

	std::vector<glm::mat4> leaves;
	std::vector<glm::uvec3> leaf_angles;
	addLeaves(leaves, leaf_angles, true);											//Generate the model matrices

	Buffer models(leaves);
	Instance particle(&flat);												//Make Particle System
	particle.bind<glm::mat4>("in_Model", &models);  //Add Matrices

	Texture tex(image::load("leaf.png"));
	Texture barktex(image::load("bark.png"));

	Shader particleShader({"shader/particle.vs", "shader/particle.fs"}, {"in_Quad", "in_Tex", "in_Model"});
	Shader barkShader({"shader/bark.vs", "shader/bark.fs"}, {"in_Quad", "in_Tex", "in_Model"});
	Shader defaultShader({"shader/default.vs", "shader/default.fs"}, {"in_Position", "in_Normal"});
	Shader depth({"shader/depth.vs", "shader/depth.fs"}, {"in_Position"});
	Shader particledepth({"shader/particledepth.vs", "shader/particledepth.fs"}, {"in_Quad", "in_Tex", "in_Model"});

	Target shadow(1600, 1600);
	Texture depthtex(1600, 1600, {GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE});
	shadow.bind(depthtex, GL_DEPTH_ATTACHMENT);

	Square3D floor;
	floor.model = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1,0,0));
	floor.model = glm::scale(floor.model, glm::vec3(1000));

	Tiny::view.pipeline = [&](){	//Setup Drawing Pipeline

		shadow.target();
		if(drawshadow){
			depth.use();
			depth.uniform("dvp", lproj*lview);
			defaultShader.uniform("model", treemesh.model);
			treemesh.render(GL_TRIANGLES);
		}
		if(leafshadow){
			particledepth.use();
			particledepth.uniform("dvp", lproj*lview);
			particledepth.texture("spriteTexture", tex);
			//addLeaves(leaves, false);						//Generate the model matrices
	                addLeaves(leaves, leaf_angles, true);					//Generate the model matrices
			models.fill<glm::mat4>(leaves);

			particle.render(GL_TRIANGLE_STRIP); 		//Render Particle System
		}

		//Prepare Render Target
		Tiny::view.target(glm::vec3(backcolor[0], backcolor[1], backcolor[2]));

		if(drawwire || drawtree){
			defaultShader.use();
			defaultShader.uniform("model", treemesh.model);
			defaultShader.uniform("projectionCamera", cam::vp);
			defaultShader.uniform("lightcolor", lightcolor);
			defaultShader.uniform("lookDir", cam::look - cam::pos);
			defaultShader.uniform("lightDir", lightpos);

			defaultShader.uniform("drawshadow", drawshadow);
			if(drawshadow){
				defaultShader.uniform("dbvp", bias*lproj*lview);
				defaultShader.texture("shadowMap", depthtex);
				defaultShader.uniform("light", lightpos);
			}

			defaultShader.uniform("drawfloor", true);
			defaultShader.uniform("drawcolor", glm::vec4(backcolor[0],backcolor[1],backcolor[2],1));
			defaultShader.uniform("model", floor.model);
			floor.render();
			defaultShader.uniform("drawfloor", false);

			defaultShader.uniform("model", treemesh.model);

			if(drawtree){
				defaultShader.uniform("drawcolor", glm::vec4(treecolor[0], treecolor[1], treecolor[2], treeopacity));
				defaultShader.uniform("wireframe", false);
			        //defaultShader.texture("spriteTexture", barktex);
			        //treemesh.render(GL_TRIANGLE_STRIP);
			        //defaultShader.render(GL_TRIANGLE_STRIP);
				//barkShader.use();
				//barkShader.texture("spriteTexture", barktex);
				//barkShader.texture("imageTexture", barktex);
				treemesh.render(GL_TRIANGLES);
			}

			if(drawwire){
				defaultShader.uniform("drawcolor", glm::vec4(wirecolor[0], wirecolor[1], wirecolor[2], 1.0));
				defaultShader.uniform("wireframe", true);
				treemesh.render(GL_LINES);
			}
		}

		if(drawleaf){

			particleShader.use();
			particleShader.texture("spriteTexture", tex);
			particleShader.texture("imageTexture", tex);
			particleShader.uniform("projectionCamera", cam::vp);
			particleShader.uniform("ff", glm::rotate(glm::mat4(1.0f), glm::radians(180-cam::rot), glm::vec3(0,1,0)));
			//particleShader.uniform("leafcolor", glm::vec4(leafcolor[0], leafcolor[1], leafcolor[2], leafopacity));
			//particleShader.uniform("leafcolor", tex);
			particleShader.uniform("lightcolor", lightcolor);
			//particleShader.uniform("model", flat.model);

			particleShader.uniform("selfshadow", selfshadow);
			if(selfshadow){
				particleShader.uniform("dbvp", bias*lproj*lview);
				particleShader.texture("shadowMap", depthtex);
				particleShader.uniform("light", lightpos);
			}

			particleShader.uniform("lookDir", cam::look - cam::pos);
			//addLeaves(leaves, true);
	                addLeaves(leaves, leaf_angles, true);						//Generate the model matrices
			models.fill<glm::mat4>(leaves);
			particle.SIZE = leaves.size();
			particle.render(GL_TRIANGLE_STRIP);                                             //Render Particle System
		}

	};



        int ffmpeg_w = xres;
        int ffmpeg_h = yres;

        std::string video_fn_out = "/tmp/output-" + curr_date_time() + ".mp4";

        // Create a vector to store the frames
        std::vector<unsigned char> pixels(ffmpeg_w * ffmpeg_h * 3);

	// Create a pipe for ffmpeg
	auto pipe = ffmpeg_create_pipe(ffmpeg_w, ffmpeg_h, video_fn_out);

	//Loop over Stuff
	Tiny::loop([&]()
        {

		if(autorotate)
			cam::pan(0.1f);

		if(!paused)
			root->grow(growthrate);

		//Update Rendering Structures
		construct(positions, normals, colors, indices);
		treemesh.index(&indices);

		models.fill<glm::mat4>(leaves);

		capture_gl_image(pixels, ffmpeg_w, ffmpeg_h);
		write_frame_to_ffmpeg(pixels, ffmpeg_w, ffmpeg_h, pipe);
	});

        // Close the ffmpeg pipe
        pclose(pipe);

	//Get rid of this thing!
	delete root;

	Tiny::quit();

	return 0;
}
