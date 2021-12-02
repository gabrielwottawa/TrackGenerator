#ifndef PROGRAM_H
#define PROGRAM_H

// Internal
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <math.h>
#include <windows.h>

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
#include "Mesh.h"
#include "ObjReader.h"

class System
{
private:
	// Screen
	const GLint WIDTH = 1280, HEIGHT = 720;
	int screenWidth, screenHeight;
	
	void defineShaders();
	void setCarShader(Material* material, Group* group);
	void setCoreShader(Material* material, Group* group);
	void drawTrack(Group* group);
	void drawCar(glm::mat4& model, Group* group, std::vector<glm::vec3*>& objPoints);
	void readMeshs(std::vector<Mesh*>& meshs);

public:
	GLFWwindow* window;
	Shader coreShader;
	Shader lightShader;
	Shader carShader;

public:
	System();
	~System();

	GLFWwindow* GLFWInit();	
	void Run(GLFWwindow* window, GLuint VAO, GLuint VAOlight, glm::vec3 cubesPositions[], vector<Mesh*> meshs, vector<vec3*> objPoints);	
	void Finish(GLuint VAO, GLuint VAO1);
};

#endif