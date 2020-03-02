#pragma once
#include "GLAD/glad.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define PPM_f 64.0f
#define PPM_s "64.0f"

//TODO: Texture manager?
//TODO: allocate the bytes?
struct Texture {
	int pixelsPerTile;
	int width, height, depth;
	unsigned char* colorBytes;

	GLuint texture;
	Texture(char* source) {
		
		int force_channels = 4;
		stbi_set_flip_vertically_on_load(true);
		colorBytes = stbi_load(source, &width, &height, &depth, force_channels);



		if (!colorBytes) {
			//error management
		}

		/*
		Check if texture is power of 2
		Area	|100 ... 0|
				 -00 ... 1
				____________
		Area - 1 0111 ... 1
				 &100 ... 0
				____________
				0000 ... 0
		0 => Was a power of 2
		!0 => Wasn't a power of 2
		*/
		int area = width * height;
		if (!(area&(area - 1))) {

		}


		glGenTextures(1, &texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, colorBytes);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//TODO: Learn about these paramaters
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	//TODO: Implement which texture to bind
	void use() {

	}

	~Texture() {

	}
};

//TODO Define along with shader code in a Shader.H
GLuint buildAndLinkShaders(std::string vertCode, std::string fragCode) {
	static int count = 0;
	const char* c_str_vertCode = vertCode.c_str();
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &c_str_vertCode, NULL);
	glCompileShader(vs);

	const char* c_str_fragCode = fragCode.c_str();
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &c_str_fragCode, NULL);
	glCompileShader(fs);

	int errorStringSize = 256;
	std::vector<char> buffer(errorStringSize);
	glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &errorStringSize);
	glGetShaderInfoLog(vs, errorStringSize, &errorStringSize, &buffer[0]);
	std::cout << count << std::endl;
	debug_printMsg("Vertex Shader error: ");
	debug_printMsg(std::string(buffer.begin(), buffer.end()));

	glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &errorStringSize);
	glGetShaderInfoLog(fs, errorStringSize, &errorStringSize, &buffer[0]);

	debug_printMsg("Fragment Shader error: ");
	debug_printMsg(std::string(buffer.begin(), buffer.end()));

	GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program, vs);
	glAttachShader(shader_program, fs);
	glLinkProgram(shader_program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	count++;
	return shader_program;
}


//Minimum number of uniform floats = 1024


/*____________________ Particle Shader Red(test)______________________________*/
//|
//|
//|_______________________________________________________________*/

std::string vert_Particle =
"#version 420\n"
//"in vec2 position;"
"in vec2 Position;"
"uniform float phaseX;"
"uniform float phaseY;"
"uniform float t;"
"out vec2 colorVec;"
"vec2 Rotate(vec2 v, float theta, float phaseX, float phaseY) {"
"	mat2 rot;"
"	rot[0] = vec2(cos(theta + phaseX), sin(theta + phaseY));"
"	rot[1] = vec2(-sin(theta + phaseY), cos(theta + phaseX));"
"	return rot*v;"
"}"

"void main() {"
" 	float distance = length(Position);"
"	float gravitation = distance*exp(-t*(1/distance));"
"	float theta_t = 1/(distance*distance)*2*3.14159*t;"
"	vec2 Pt = gravitation*Rotate(Position, theta_t, phaseX, phaseY);"
"	gl_PointSize = 1;"
"	gl_Position = vec4(Pt.x*720/1280, Pt.y, 0.0, 1.0);"
"}";

std::string frag_Particle =
"#version 420\n"
"in vec2 colorVec;"
"out vec4 frag_color;\n"
"void main() {\n"
"	vec3 color = vec3(.09, .09, 0.6);"
"	frag_color = vec4(color, 1.0);\n "
"}\n";
