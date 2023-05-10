#include <string>
#include <vector>

#include <boost/log/trivial.hpp>

void asdf();
FILE * ffmpeg_create_pipe(int ffmpeg_w, int ffmpeg_h, std::string video_fn_out);
void capture_gl_image(std::vector<unsigned char> & pixels, int ffmpeg_w, int ffmpeg_h);
void write_frame_to_ffmpeg(std::vector<unsigned char> & pixels, int ffmpeg_w, int ffmpeg_h, FILE *pipe);
