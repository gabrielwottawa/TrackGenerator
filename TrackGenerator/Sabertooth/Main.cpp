#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <math.h>

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <SOIL.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "Shader.h"
#include "Time.h"
#include "System.h"

#define EXIT_FAILURE -1
#define EXIT_SUCCESS 0

System sys;

int main() {
	
	GLFWwindow* window = sys.GLFWInit();
	
	sys.Run(sys.coreShader, window);
	
	sys.Finish();

	return 0;
}