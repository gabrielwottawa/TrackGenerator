#include <cstdlib>
#include "System.h"
#include "Mesh.h"

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void validateKeys(GLFWwindow* window);
bool checkCollision(glm::vec3 cubePosition, glm::vec3 shotPosition);
void drawCubes(unsigned int VAO1, glm::vec3  cubePositions[12], Shader coreShader);
void handleShot(Shader coreShader, glm::vec3 cubePositions[], vector<vec3*> objPoints);
void setCoreShaderLights(Shader shader, glm::mat4& proj, glm::mat4& view);

glm::vec3 positionCamera = glm::vec3(0.0f, 2.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yawa = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitcha = 0.0f;
float lastX = 800.0f / 2.0f;
float lastY = 600.0 / 2.0f;
float fov = 45.0f;

float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

bool hasShot = false;
int iterations = 0;
glm::vec3 shotMove = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 lastFrontPosition = glm::vec3(0.0f, 0.0f, 0.0f);

int cont = 0;

glm::vec3 lightPosition(0.0f, 2.2f, 0.0f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

int positionTrack = 0;

bool collision[13] = {
	false, false, false, false, false,
	false, false, false, false, false, 
	false, false, false
};

bool running = false;

System::System() {}

System::~System() {}

GLFWwindow* System::GLFWInit()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "FPS_Game", nullptr, nullptr);

	if (window == nullptr) {
		std::cout << "Failed to create GLFW Window" << std::endl;
		glfwTerminate();

		return 0;
	}
	glfwMakeContextCurrent(window);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);

	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {
		std::cout << "Failed no init GLEW." << std::endl;
		return 0;
	}
	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
	glViewport(0, 0, screenWidth, screenHeight);

	defineShaders();
	coreShader.Use();

	// Obtendo as informações de versão
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	return window;
}

void System::defineShaders()
{
	coreShader = Shader("Shaders/Core/core.vert", "Shaders/Core/core.frag");
	lightShader = Shader("Shaders/Core/core2.vert", "Shaders/Core/core2.frag");
	carShader = Shader("Shaders/Core/core3.vert", "Shaders/Core/core3.frag");
}

void System::Run(GLFWwindow* window, GLuint VAO, GLuint VAOlight, glm::vec3 cubePositions[], vector<Mesh*> meshs, vector<vec3*> objPoints)
{
	coreShader.Use();
	readMeshs(meshs);

	// game loop
	while (!glfwWindowShouldClose(window)) {

		glfwPollEvents();
		
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		validateKeys(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		coreShader.Use();

		glm::mat4 view(1.0f);
		view = glm::lookAt(positionCamera, positionCamera + cameraFront, cameraUp);
		
		glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 1000.0f);
		
		setCoreShaderLights(coreShader, proj, view);
		setCoreShaderLights(carShader, proj, view);

		glm::mat4 model(1.0f);
		coreShader.setMatrix4fv("model", model);
		carShader.setMatrix4fv("model", model);

		glUniform1i(glGetUniformLocation(coreShader.program, "tex"), 0);
		for (Mesh* mesh : meshs) {			
			for (Group* group : mesh->getGroups()) {				
				if (group->getName() == "pista")
					drawTrack(group);				
				else 
					drawCar(model, group, objPoints);				
			}
		}

		drawCubes(VAO, cubePositions, coreShader);

		lightShader.Use();
		lightShader.setMatrix4fv("projection", proj);
		lightShader.setMatrix4fv("view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPosition);

		glUniform1i(glGetUniformLocation(coreShader.program, "tex"), 1);
		//coreShader.Use();
		glBindVertexArray(VAOlight);

		
		model = glm::translate(model, cubePositions[0]);
		lightShader.setMatrix4fv("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);										

		glUniform1i(glGetUniformLocation(coreShader.program, "tex"), 2);
		glBindVertexArray(VAO);
		coreShader.Use();
		handleShot(coreShader, cubePositions, objPoints);
		
		positionTrack++;
		glfwSwapBuffers(window);
	}
}

void System::drawTrack(Group* group)
{
	coreShader.UseTexture(group->getName());
	glBindVertexArray(group->getVAO());
	glDrawArrays(GL_TRIANGLES, 0, group->getNumVertices());
	glBindVertexArray(0);
}

void System::drawCar(glm::mat4& model, Group* group, std::vector<glm::vec3*>& objPoints)
{
	model = glm::mat4(1.0f);
	carShader.UseTexture(group->getName());
	glBindVertexArray(group->getVAO());
	if (positionTrack == objPoints.size())
		positionTrack = 0;

	glm::vec3 a = (*objPoints[positionTrack]);
	glm::vec3 b = (*objPoints[(positionTrack == objPoints.size() - 1) ? 0 : positionTrack + 1]);

	GLfloat dx = b.x - a.x;
	GLfloat dy = b.y - a.y;
	GLfloat dz = b.z - a.z;

	GLfloat angle = -glm::atan(dz, dx);

	if (!collision[12]) {
		model = translate(model, *objPoints[positionTrack]);
		model = rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
		model = scale(model, glm::vec3(0.5, 0.5, 0.5));

		coreShader.setMatrix4fv("model", model);
		glDrawArrays(GL_TRIANGLES, 0, group->getNumVertices());
		glBindVertexArray(0);
	}
}

void System::readMeshs(std::vector<Mesh*>& meshs)
{
	for (Mesh* mesh : meshs) {
		for (Group* group : mesh->getGroups()) {

			Material* material = mesh->getMaterial(group->getMaterial());

			if (group->getName() == "pista")
				setCoreShader(material, group);
			else
				setCarShader(material, group);

			vector<float> vertices;
			vector<float> normais;
			vector<float> textures;

			for (Face* face : group->getFaces()) {
				for (int verticeID : face->getVertices()) {
					glm::vec3* vertice = mesh->vertice(verticeID - 1);
					vertices.push_back(vertice->x);
					vertices.push_back(vertice->y);
					vertices.push_back(vertice->z);

					group->increaseNumVertices();
				}

				for (int normalID : face->getNormais()) {
					glm::vec3* normal = mesh->normal(normalID - 1);
					normais.push_back(normal->x);
					normais.push_back(normal->y);
					normais.push_back(normal->z);
				}

				for (int textureID : face->getTextures()) {
					glm::vec2* texture = mesh->texture(textureID - 1);
					textures.push_back(texture->x);
					textures.push_back(texture->y);
				}
			}

			GLuint VBOvertices, VBOnormais, VBOtextures, VAO;
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBOvertices);
			glGenBuffers(1, &VBOnormais);
			glGenBuffers(1, &VBOtextures);

			glBindVertexArray(VAO);

			glBindBuffer(GL_ARRAY_BUFFER, VBOvertices);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);

			glBindBuffer(GL_ARRAY_BUFFER, VBOnormais);
			glBufferData(GL_ARRAY_BUFFER, normais.size() * sizeof(float), normais.data(), GL_STATIC_DRAW);

			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(1);

			glBindBuffer(GL_ARRAY_BUFFER, VBOtextures);
			glBufferData(GL_ARRAY_BUFFER, textures.size() * sizeof(float), textures.data(), GL_STATIC_DRAW);

			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(2);

			group->setVAO(&VAO);
			glBindVertexArray(0); // Unbind VAO
		}
	}
}

void setCoreShaderLights(Shader shader, glm::mat4& proj, glm::mat4& view)
{
	shader.setMatrix4fv("projection", proj);
	shader.setMatrix4fv("view", view);
	shader.setVec3("ligthDiff", glm::vec3(0.5f, 0.5f, 0.5f));
	shader.setVec3("ligthSpec", glm::vec3(1.0f, 1.0f, 1.0f));
	shader.setVec3("lightColor", lightColor);
	shader.setVec3("objectColor", glm::vec3(1.0f, 1.0f, 1.0f));
	shader.setVec3("viewPos", positionCamera);
}

void System::setCarShader(Material* material, Group* group)
{
	carShader.LoadTexture(_strdup(material->getTexture().c_str()), "texture1", group->getName());
	carShader.setVec3("materialAmbient", vec3(material->getAmbient()->x, material->getAmbient()->y, material->getAmbient()->z));
	carShader.setVec3("materialDiffuse", vec3(material->getDiffuse()->x, material->getDiffuse()->y, material->getDiffuse()->z));
	carShader.setVec3("materialSpecular", vec3(material->getSpecular()->x, material->getSpecular()->y, material->getSpecular()->z));
	carShader.setFloat("materialShininess", material->getShininess());
}

void System::setCoreShader(Material* material, Group* group)
{
	coreShader.LoadTexture(_strdup(material->getTexture().c_str()), "texture1", group->getName());
	coreShader.setVec3("materialAmbient", vec3(material->getAmbient()->x, material->getAmbient()->y, material->getAmbient()->z));
	coreShader.setVec3("materialDiffuse", vec3(material->getDiffuse()->x, material->getDiffuse()->y, material->getDiffuse()->z));
	coreShader.setVec3("materialSpecular", vec3(material->getSpecular()->x, material->getSpecular()->y, material->getSpecular()->z));
	coreShader.setFloat("materialShininess", material->getShininess());
}

void handleShot(Shader coreShader, glm::vec3 cubePositions[], vector<vec3*> objPoints)
{		
	if (hasShot) {
		if (iterations == 0) {
			glm::vec3 temp = positionCamera;
			temp[1] = 1.0f;
			shotMove = temp;
			lastFrontPosition = cameraFront;
		}

		iterations += 1;
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, shotMove);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		float shotSpeed = 80 * deltaTime;
		shotMove += shotSpeed * lastFrontPosition;
		coreShader.setMatrix4fv("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		if (iterations >= 200) {
			hasShot = false;
			iterations = 0;
		}

		for (int i = 0; i < 12; i++) {
			if (!collision[i]) 
				collision[i] = checkCollision(cubePositions[i], shotMove);			
		}

		if (!collision[12])
			collision[12] = checkCollision(*objPoints[positionTrack], shotMove);
	}
}

void drawCubes(unsigned int VAO1, glm::vec3  cubePositions[12], Shader coreShader)
{
	//------------------Desenha cubos---------------------------------
	// Carrega cor dos cubos

	coreShader.setVec3("objectColor", glm::vec3(0.0f, 0.4f, 1.0f));
	glUniform1i(glGetUniformLocation(coreShader.program, "tex"), 1);
	coreShader.Use();
	glBindVertexArray(VAO1);
	for (unsigned int i = 0; i < 12; i++)
	{
		if (!collision[i]) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			coreShader.setMatrix4fv("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}
	//----------------------------------------------------------------
}

// Valida todas as teclas pressionadas
// ---------------------------------------------------------------------------------------------------------
void validateKeys(GLFWwindow * window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	int multiplier = running ? 20 : 5;

	//Trata movimento do "personagem"		
	float cameraSpeed = multiplier * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		positionCamera += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		positionCamera -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		positionCamera -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		positionCamera += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) 
		running = !running;

	//Trata clique do mouse para o tiro
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
		hasShot = true;
}

void System::Finish(GLuint VAO, GLuint VAO1)
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &VAO1);
	coreShader.Delete();
	lightShader.Delete();
	glfwTerminate();
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.4f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yawa += xoffset;
	pitcha += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitcha > 89.0f)
		pitcha = 89.0f;
	if (pitcha < -89.0f)
		pitcha = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yawa)) * cos(glm::radians(pitcha));
	front.y = sin(glm::radians(pitcha));
	front.z = sin(glm::radians(yawa)) * cos(glm::radians(pitcha));
	cameraFront = glm::normalize(front);
}

// chech collision
bool checkCollision(glm::vec3 cubePosition, glm::vec3 shotPosition) {
	float radC = 0.8f;
	float radS = 0.4f;
	float rad = radC + radS;
	float x = cubePosition.x - shotPosition.x;
	float y = cubePosition.y - shotPosition.y;
	float z = cubePosition.z - shotPosition.z;

	float interesectC = pow(x, 2) + pow(y, 2) + pow(z, 2);
	float distC = pow(rad, 2);
	if (interesectC < distC) {
		return true;
	}
	else {
		return false;
	}
}