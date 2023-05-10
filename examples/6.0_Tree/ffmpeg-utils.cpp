#include "ffmpeg-utils.h"
#include <iostream>

#include <SDL2/SDL_opengl.h>

void asdf()
{
	std::cout << "asdf" << std::endl;
}

FILE * ffmpeg_create_pipe(int ffmpeg_w, int ffmpeg_h, std::string video_fn_out)
{
	// ffmpeg command to convert images to video
	std::string ffmpeg_cmd = "ffmpeg -y -f rawvideo -pix_fmt rgb24 -s " + std::to_string(ffmpeg_w) + "x" + std::to_string(ffmpeg_h) + " -i - -vf vflip -f mp4 -codec:v libx264 -preset ultrafast -crf 23 -pix_fmt yuv420p " + video_fn_out;

	BOOST_LOG_TRIVIAL(info) << "Sending command line to ffmpeg: " << ffmpeg_cmd << std::endl;

	// Create a pipe for ffmpeg
	FILE *pipe = popen(ffmpeg_cmd.c_str(), "w");
	if (!pipe)
	{
		BOOST_LOG_TRIVIAL(error) << "Error opening pipe to ffmpeg command" << std::endl;
		return NULL;
	}

	return pipe;
}

void capture_gl_image(std::vector<unsigned char> & pixels, int ffmpeg_w, int ffmpeg_h)
{
	// Read the pixels from the framebuffer
	glReadPixels(0, 0, ffmpeg_w, ffmpeg_h, GL_RGB, GL_UNSIGNED_BYTE, &pixels[0]);
	BOOST_LOG_TRIVIAL(debug) << "Captured " << pixels.size() << " bytes from framebuffer" << std::endl;
}

void write_frame_to_ffmpeg(std::vector<unsigned char> & pixels, int ffmpeg_w, int ffmpeg_h, FILE *pipe)
{
	BOOST_LOG_TRIVIAL(debug) << "Writing " << pixels.size() << " bytes to ffmpeg" << std::endl;

	// Write the pixels to the pipe
	int written_bytes = fwrite(pixels.data(), sizeof(unsigned char), pixels.size(), pipe);
	if (written_bytes != pixels.size())
	{
		BOOST_LOG_TRIVIAL(error) << "Error sending image buffer to ffmpeg, only " << written_bytes << " bytes written" << std::endl;
	}
}
