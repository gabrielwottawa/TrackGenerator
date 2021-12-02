#ifndef PROGRAM_H
#define PROGRAM_H
#define _USE_MATH_DEFINES

// Internal
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <math.h>
#include <vector>

// External Libs
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <SOIL.h>

// GLM Includes
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

// Headers
#include "Shader.h"
#include "Time.h"

using namespace std;

class System
{
private:
	// Screen
	const GLint WIDTH = 1280, HEIGHT = 720;
	int screenWidth, screenHeight;

public:
	GLFWwindow* window;
	Shader coreShader;

public:
	System();
	~System();

	GLFWwindow* GLFWInit();	
	void Run(Shader shader, GLFWwindow* window);	
	void Finish();
};

#endif