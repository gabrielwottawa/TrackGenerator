// Internal
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <math.h>

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
#include "AssetManager.h"
#include "Time.h"
#include "System.h"

#define EXIT_FAILURE -1
#define EXIT_SUCCESS 0

// program
System sys;

int main() {
	// window init
	GLFWwindow* window = sys.GLFWInit();

	// run
	sys.Run(sys.coreShader, window);

	// finish
	sys.Finish();

	return 0;
}