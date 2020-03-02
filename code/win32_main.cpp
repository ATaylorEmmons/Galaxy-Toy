
#include <string>
#include <vector>
#include <random>

#include <Windows.h>

#include "Debug.h"

#include "GLAD/glad.h"
#include "GLFW/glfw3.h"
#include "EngineMath.h"
#include "Shaders.h"

struct Color {
	float values[3];
	float* operator[](int index) { return ((&values)[index]); }
};


void window_framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}


//TODO: Put these in an input structure.
//	Maybe make use of the repeat action

struct Keypress {
	bool IsDown;
	bool wasDown;
	int repeatCount;
};

Global Keypress W = {0};
Global Keypress S = {0};
Global Keypress A = {0};
Global Keypress D = {0};
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_W) {
		if (action == GLFW_PRESS) {
			W.IsDown = true;
		}
		if (action == GLFW_RELEASE) {
			W.IsDown = false;
		}
	} 

	if (key == GLFW_KEY_S) {
		if (action == GLFW_PRESS) {
			S.IsDown = true;
		}
		if (action == GLFW_RELEASE) {
			S.IsDown = false;
		}
	}
		
}



int main() {
	

	float windowWidth = 1280.0f;
	float windowHeight = 720.0f;

	if (!glfwInit()) {
		debug_printMsg("Failed to initilize window (GLFW3.2)");
	} 

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_SAMPLES, 1);

	GLFWwindow* window = glfwCreateWindow((int)windowWidth,(int)windowHeight, "Two Spiral Galaxy", NULL, NULL);

	if (!window) {
		debug_printMsg("Could not create a window");
	}


	/* Init opengl */
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, window_framebufferSizeCallback);
	glfwSetKeyCallback(window, key_callback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		debug_printMsg("Failed to load OpenGL functions");
	}

	const GLubyte* glVersion = glGetString(GL_VERSION);
	debug_printMsg((char*)glVersion);




	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glViewport(0, 0, 1024, 768);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	double start_t = 0.0;
	double end_t = 0.0;

	// TODO: Implement Kahan sum?
	double runningTime = 0.0;



	float dt = 0.0f;
	glfwSetTime(0.0);
	
	//TODO: Particle Color, position?
	GLint particleProgram = buildAndLinkShaders(vert_Particle, frag_Particle);
	int u_time = glGetUniformLocation(particleProgram, "t");
	int u_phaseX = glGetUniformLocation(particleProgram, "phaseX");
	int u_phaseY = glGetUniformLocation(particleProgram, "phaseY");

	const uint32_t PARTICLE_COUNT = 50000;
	GLuint positionVBO;
	GLuint particleVAO;
	float particleLife_seconds = 300.0f;
	float particlePos[PARTICLE_COUNT*2] = {0};


/*_________________ Galaxy Controls____________________________*/

	float timeOffset = 10.0f;
	float timeScale = .01f;

	float phaseX = 0.0f;
	float phaseY = 0.0f;
	
	float shiftX = 0.0f;
	float shiftY = 0.0f;

	float posX;
	float posY;



	std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_real_distribution<float> nOneToOne(-1.0f, 1.0f);
	std::uniform_real_distribution<float> ZeroToOne(0.0f, 1.0f);

	std::normal_distribution<float> normalDist(0.0f, .5f);

	std::normal_distribution<float> radiusVariance(.9f, .2f);

	float counter = 0;
	for(int i = 0; i < PARTICLE_COUNT*2; i+=2) {
		

		

		float radius = nOneToOne(gen);//radiusVariance(gen);
		float prongs = 2.0f;

		float theta = (2*PI/prongs)*counter + normalDist(gen);

	
		Vector2 pos = {4*radius*cos(theta), 2*sin(theta)*radius};

		particlePos[i] = pos.x;
		particlePos[i + 1] = pos.y;

		counter += 1.0f;
	}


	glGenBuffers(1, &positionVBO);
	glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*2*PARTICLE_COUNT, particlePos, GL_STATIC_DRAW);

	glGenVertexArrays(1, &particleVAO);
	glBindVertexArray(particleVAO);

	glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);




	while (!glfwWindowShouldClose(window)) {

		float currentTime = (float)(runningTime);
		start_t = glfwGetTime();

/* 		Input Defs */

		W.wasDown = W.IsDown;
		S.wasDown = S.IsDown;

		glfwPollEvents();

		if (W.wasDown && W.IsDown) {
			W.repeatCount++;
		}
		else {
			W.repeatCount = 0;
		}

		if (S.wasDown && S.IsDown) {
			S.repeatCount++;
		}
		else {
			S.repeatCount = 0;
		}
	

		if (W.IsDown) {
			phaseX += .01f;
		}

		if (S.IsDown) {
			phaseY += .01f;
		}
		
		glClear(GL_COLOR_BUFFER_BIT);

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);


		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glEnable(GL_PROGRAM_POINT_SIZE);
		glUseProgram(particleProgram);
		glBindVertexArray(particleVAO);
		glUniform1f(u_phaseX, phaseX);
		glUniform1f(u_phaseY, phaseY);
		glUniform1f(u_time, ((float)runningTime)*timeScale);
		glDrawArrays(GL_POINTS, 0, PARTICLE_COUNT);
		glDisable(GL_PROGRAM_POINT_SIZE);
		glDisable(GL_BLEND);
		
	

		glfwSwapBuffers(window);	

		end_t = glfwGetTime();
		runningTime = end_t;
		
		dt = (float)(end_t - start_t);

	}



	glDeleteProgram(particleProgram);
	glfwTerminate();

	return 0;
}