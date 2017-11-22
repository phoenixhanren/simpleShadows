#include <iostream>
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include "Shader.h"
#include "EulerFPSCamera.h"
#include "Model.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <list>
#include <vector>
#include <utility>
#include <map>

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>
using std::list;
using std::cin;
using std::cout;
using std::endl;
using std::pair;
using std::vector;
using std::map;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void setup();
void render();
void clean();
void drawModelVolumes();
list<pair<glm::vec3, glm::vec3>> 
slihouetteDetemination(const vector<GLfloat> &vertices, const glm::vec3 &lightPosition,const glm::mat4 &projection, const glm::mat4 &view, const glm::mat4 &model);

//struct Edge
//{
//	Edge(GLuint _a, GLuint _b)
//	{
//		assert(_a != _b);
//
//		if (_a < _b)
//		{
//			a = _a;
//			b = _b;
//		}
//		else
//		{
//			a = _b;
//			b = _a;
//		}
//	}
//
//	void Print()
//	{
//		printf("Edge %d %d\n", a, b);
//	}
//
//	GLuint a;
//	GLuint b;
//};

struct Neighbors
{
	GLuint n1;
	GLuint n2;

	Neighbors()
	{
		n1 = n2 = (GLuint)-1;
	}

	void AddNeigbor(GLuint n)
	{
		if (n1 == -1) {
			n1 = n;
		}
		else if (n2 == -1) {
			n2 = n;
		}
		else {
			assert(0);
		}
	}

	GLuint GetOther(GLuint me) const
	{
		if (n1 == me) {
			return n2;
		}
		else if (n2 == me) {
			return n1;
		}
		else {
			assert(0);
		}

		return 0;
	}
};


struct CompareVectors
{
	bool operator()(const aiVector3D& a, const aiVector3D& b)
	{
		if (a.x < b.x) {
			return true;
		}
		else if (a.x == b.x) {
			if (a.y < b.y) {
				return true;
			}
			else if (a.y == b.y) {
				if (a.z < b.z) {
					return true;
				}
			}
		}

		return false;
	}
};

//struct Face
//{
//	GLuint Indices[3];
//
//	Face(GLuint x, GLuint y, GLuint z) {
//		Indices[0] = x;
//		Indices[1] = y;
//		Indices[2] = z;
//	}
//
//	GLuint GetOppositeIndex(const Edge& e) const
//	{
//		for (GLuint i = 0; i < 3; i++) {
//			GLuint Index = Indices[i];
//
//			if (Index != e.a && Index != e.b) {
//				return Index;
//			}
//		}
//
//		assert(0);
//
//		return 0;
//	}
//};
void findAdjacencies(const vector<Face>& paiMesh, vector<unsigned int>& Indices);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// camera
EulerFPSCamera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

GLFWwindow* window;

unsigned int planeVAO, planeVBO;
unsigned int quadVAO, quadVBO;
unsigned int cubeVAO, cubeVBO;
unsigned int eleCubeVAO, eleCubeVBO, eleCubeEBO;
Model * ourModel;
//Model ourModel("D:/daily exercise/opengl/models/nanosuit/nanosuit-Arms.obj", true);
//Model ourModel("D:/daily exercise/opengl/models/monkey.ply", true);
unsigned int woods;
glm::vec3 cubePositions[3];
GLfloat cubeVertices[] = {
		// Back face
		//pos				 //normal			//text
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
		0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,  // top-right
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,  // bottom-left
		-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,// top-left
		// Front face
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,  // top-right
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom-left
		// Left face
		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
		-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-left
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
		-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
		// Right face
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
		0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right         
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-right
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // top-left
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left     
		// Bottom face
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
		0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,// bottom-left
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
		-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
		// Top face
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
		0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right     
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f // bottom-left        
};
vector<GLfloat>cubeVectors(cubeVertices, cubeVertices + sizeof(cubeVertices) / sizeof(GLfloat));
Shader shadowShader;
Shader silhouetteShader;
Shader lightShader; 
Shader silTestShader;
Shader volumeShader;
glm::vec3 lightPos(10.0f, 10.0f, 10.0f);

GLfloat cubeVerticesIndex[] = {
	-0.5f, -0.5f, -0.5f,
	0.5f, 0.5f, -0.5f, 
	0.5f, -0.5f, -0.5f,         
    -0.5f, 0.5f, -0.5f,
    -0.5f, -0.5f, 0.5f, 
    0.5f, -0.5f, 0.5f,   
    0.5f, 0.5f, 0.5f,   
	-0.5f, 0.5f, 0.5f,   
};


GLuint cubeIndices[] = {
	0, 1, 2, 
	1, 0, 3,

	4, 5, 6, 
	6, 7, 4,

	7, 3, 0,
	0, 4, 7,

	6, 2, 1, 
	2, 6, 5,

	0, 2, 5, 
	5, 4, 0,

	3, 6, 1, 
	6, 3, 7
};


vector<Face> mesh = {
	Face(0, 1, 2),
	Face(1, 0, 3),
	Face(4, 5, 6),
	Face(6, 7, 4),
	Face(7, 3, 0),
	Face(0, 4, 7),
	Face(6, 2, 1),
	Face(2, 6, 5),
	Face(0, 2, 5),
	Face(5, 4, 0),
	Face(3, 6, 1),
	Face(6, 3, 7)
};
vector<unsigned int> indi;


int main()
{
	setup();
	shadowShader = Shader("Shaders//shadowShader.vs", "Shaders//shadowShader.fs");
	silhouetteShader = Shader("Shaders//silhouetteShader.vs", "Shaders//silhouetteShader.fs", "Shaders//silhouetteShader.gs");
	silTestShader = Shader("Shaders//silhouetteShader.vs", "Shaders//silhouetteShader.fs");
	lightShader = Shader("Shaders//lightShader.vs", "Shaders//lightShader.fs");
	volumeShader = Shader("Shaders//shadowVolume.vs", "Shaders//shadowVolume.fs", "Shaders//shadowVolume.gs");
	ourModel =  new Model("D:/daily exercise/opengl/models/monkey.ply", true);
	while (!glfwWindowShouldClose(window))
	{
		render();
	}

	clean();
	delete ourModel;
	return 0;
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; 

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void setup()
{

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewExperimental = GL_TRUE;
	if (!glewInit() == GLEW_OK)
	{
		std::cout << "Failed to init GLEW" << std::endl;
		glfwTerminate();
	}

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);

	GLfloat planeVertices[] = {
		// Positions          // Normals         // Texture Coords
		25.0f, -0.5f, 25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,
		-25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f,
		-25.0f, -0.5f, 25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

		25.0f, -0.5f, 25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,
		25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 25.0f,
		-25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f
	};

	GLfloat quadVertices[] = {
		// Positions        // Texture Coords
		-1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
		1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
		1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
	};

	/*GLfloat cubeVertices[] = {
		// Back face
		//pos				 //normal			//text
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
		0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,  // top-right
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,  // bottom-left
		-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,// top-left
		// Front face
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,  // top-right
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom-left
		// Left face
		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
		-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-left
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
		-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
		// Right face
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
		0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right         
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-right
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // top-left
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left     
		// Bottom face
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
		0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,// bottom-left
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
		-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
		// Top face
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
		0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right     
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f // bottom-left        
	};*/

	cubePositions[0] = glm::vec3(0.0f, 1.5f, 0.0f);
	cubePositions[1] = glm::vec3(2.0f, 0.0f, 1.0f);
	cubePositions[2] = glm::vec3(-1.0f, 0.0f, 2.0f);

	
	findAdjacencies(mesh, indi);
	int count = 0;
	for (auto &i : indi)
	{
		cout << i << " ";
		if ((++count) % 6 == 0)
		{
			cout << endl;
		}
	}
	cout << endl;

	cout << indi.size() << endl;

#pragma region VAOS
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void *)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void *)(3 * sizeof(GL_FLOAT)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void *)(6 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void *)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void *)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void *)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void *)(3 * sizeof(GL_FLOAT)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void *)(6 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &eleCubeVAO);
	glGenBuffers(1, &eleCubeVBO);
	glGenBuffers(1, &eleCubeEBO);
	glBindVertexArray(eleCubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, eleCubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerticesIndex), cubeVerticesIndex, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleCubeEBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indi.size() * sizeof(indi[0]), &indi[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

#pragma endregion

	woods = TextureFromFile("wood.png", "D:\\daily exercise\\opengl\\simpleShadows\\simpleShadows\\Debug");

}

void render()
{
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	double t = glfwGetTime();
	//lightPos = glm::vec3(10.0f * sin(t), 10.0f , 10.0f * cos(t));
	//清除缓冲区，包括模板缓冲
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//第一遍 渲染环境光，深度值

	//第二遍 渲染模板值

	//第三遍 渲染光源光照，依据模板值判断阴影
	list<pair<glm::vec3, glm::vec3>> silhouetteEdges;
	glDepthFunc(GL_LESS);
	shadowShader.use();
	glm::mat4 model;
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
		(float)SCR_WIDTH / SCR_HEIGHT, 1.0f, 100.0f);
	shadowShader.set_mat4("projection", projection);
	shadowShader.set_mat4("view", view);
	shadowShader.set_mat4("model", model);
	shadowShader.set_vec3("lightPos", lightPos);
	shadowShader.set_vec3("viewPos", camera.Position);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, woods);
	shadowShader.set_int("diffuseTexture", 0);
	glBindVertexArray(planeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(cubeVAO);

	for (unsigned int i = 0; i != cubePositions->length(); ++i)
	{
		model = glm::mat4();
		model = glm::translate(model, cubePositions[i]);
		shadowShader.set_mat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		//silhouetteEdges.splice(silhouetteEdges.end(), slihouetteDetemination(cubeVectors, lightPos, projection, view, model));
	}
	lightShader.use();
	lightShader.set_mat4("projection", projection);
	lightShader.set_mat4("view", view);
	model = glm::mat4();
	model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
	model = glm::translate(model, lightPos);
	lightShader.set_mat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	shadowShader.use();
	model = glm::mat4();
	model = glm::scale(model, glm::vec3(0.1, 0.1, 0.1));
	shadowShader.set_mat4("projection", projection);
	shadowShader.set_mat4("view", view);
	shadowShader.set_mat4("model", model);
	shadowShader.set_vec3("lightPos", lightPos);
	shadowShader.set_vec3("viewPos", camera.Position);
	ourModel->Draw(shadowShader);


	glm::vec3 tempLightPos = projection * view * glm::vec4(lightPos, 1.0f);

	silhouetteShader.use();
	silhouetteShader.set_mat4("view", view);
	silhouetteShader.set_mat4("projection", projection);
	silhouetteShader.set_vec3("lightPos", lightPos);
	silhouetteShader.set_vec3("finalLightPos", tempLightPos);
	glDepthFunc(GL_LEQUAL);
	glLineWidth(5.0f);
	glBindVertexArray(eleCubeVAO);

	for (unsigned int i = 0; i != cubePositions->length(); ++i)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleCubeEBO);
		model = glm::mat4();
		model = glm::translate(model, cubePositions[i]);
		silhouetteShader.set_mat4("model", model);
		//glDrawArrays(GL_TRIANGLES_ADJACENCY, 0, 36);
		//glDrawElementsBaseVertex(GL_TRIANGLES_ADJACENCY, indi.size(), GL_UNSIGNED_INT, (void *)0
		//	,0);
		//silhouetteEdges.splice(silhouetteEdges.end(), slihouetteDetemination(cubeVectors, lightPos, projection, view, model));
		glDrawElements(GL_TRIANGLES_ADJACENCY, indi.size(), GL_UNSIGNED_INT, 0);
		//glDrawElementsBaseVertex(GL_TRIANGLES_ADJACENCY, indi.size(),
		//	GL_UNSIGNED_INT,)
	}
	model = glm::mat4();
	model = glm::scale(model, glm::vec3(0.1, 0.1, 0.1));
	silhouetteShader.set_mat4("model", model);
	ourModel->Draw(silhouetteShader);
#pragma region silTest
	silTestShader.use();
	silTestShader.set_mat4("view", view);
	silTestShader.set_mat4("projection", projection);
	for (unsigned int i = 0; i != cubePositions->length(); ++i)
	{
		model = glm::mat4();
		model = glm::translate(model, cubePositions[i]);
		silTestShader.set_mat4("model", model);
		//glDrawArrays(GL_TRIANGLES_ADJACENCY, 0, 36);
		//glDrawElementsBaseVertex(GL_TRIANGLES, indi.size(), GL_UNSIGNED_INT, (void *)0
		//	, 0);
		//silhouetteEdges.splice(silhouetteEdges.end(), slihouetteDetemination(cubeVectors, lightPos, projection, view, model));
		//glDrawElements(GL_TRIANGLES, indi.size(), GL_UNSIGNED_INT, 0);
		//glDrawElementsBaseVertex(GL_TRIANGLES_ADJACENCY, indi.size(),
		//	GL_UNSIGNED_INT,)
	}

#pragma endregion
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//glLineWidth(1.0f);
	//glEnable(GL_DEPTH_CLAMP);
	volumeShader.use();
	volumeShader.set_vec3("lightPos", lightPos);
	volumeShader.set_mat4("view", view);
	volumeShader.set_mat4("projection", projection);
	glDepthFunc(GL_LESS);
	for (unsigned int i = 0; i != cubePositions->length(); ++i)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleCubeEBO);
		model = glm::mat4();
		model = glm::translate(model, cubePositions[i]);
		volumeShader.set_mat4("model", model);
		//glDrawArrays(GL_TRIANGLES_ADJACENCY, 0, 36);
		//glDrawElementsBaseVertex(GL_TRIANGLES_ADJACENCY, indi.size(), GL_UNSIGNED_INT, (void *)0
		//	,0);
		//silhouetteEdges.splice(silhouetteEdges.end(), slihouetteDetemination(cubeVectors, lightPos, projection, view, model));
		//glDrawElements(GL_TRIANGLES_ADJACENCY, indi.size(), GL_UNSIGNED_INT, 0);
		//glDrawElementsBaseVertex(GL_TRIANGLES_ADJACENCY, indi.size(),
		//	GL_UNSIGNED_INT,)
	}

	//glDisable(GL_DEPTH_CLAMP);

	glBindVertexArray(0);

	processInput(window);
	glfwSwapBuffers(window);
	glfwPollEvents();

	for (auto &edge : silhouetteEdges)
	{
		//cout << "(" << edge.first.x << ", " << edge.first.y << ", " << edge.first.z << ")->";
		//cout << "(" << edge.second.x << ", " << edge.second.y << ", " << edge.second.z << ")" << endl;
	}
	//cout << "==============================================" << endl;

}

void clean()
{

}

void drawModelVolumes()
{

}

list<pair<glm::vec3, glm::vec3>> slihouetteDetemination(const vector<GLfloat> &vertices, const glm::vec3 &lightPosition,
	const glm::mat4 &projection, const glm::mat4 &view, const glm::mat4 &model)
{
	list<pair<glm::vec3, glm::vec3>> slihouetteEdge;
	vector<glm::vec3> points;

	glm::mat4 transformMat = projection * view * model;

	for (int i = 0; i < vertices.size(); ++i)
	{
		GLfloat i1 = vertices[i++];
		GLfloat i2 = vertices[i++];
		GLfloat i3 = vertices[i++];
		i = i + 4;
		points.push_back(glm::vec3(transformMat * (glm::vec4(glm::vec3(i1, i2, i3), 1.0))));
	}

	for (int i = 0; i != points.size(); ++i)
	{
		glm::vec3 i1 = points[i++];
		glm::vec3 i2 = points[i++];
		glm::vec3 i3 = points[i];
		pair<glm::vec3, glm::vec3> edge(i2 - i1, i3 - i2);
		glm::vec3 normal = glm::cross(edge.first, edge.second);
		glm::vec3 lightDir = lightPosition - i1;

		if (glm::dot(normal , lightDir) > 0)
		{
			size_t originalSize = slihouetteEdge.size();
			slihouetteEdge.remove_if([&](const pair<glm::vec3, glm::vec3>& e)->bool{
				if ((e.first == edge.first && e.second == edge.second) || (e.first == edge.second && e.second == edge.first))
				{
					return true;
				}
				return false;
			});
			if (originalSize == slihouetteEdge.size())
			{
				slihouetteEdge.push_back(edge);
			}
		}
	}

	return slihouetteEdge;

}

void findAdjacencies(const vector<Face>& mesh, vector<unsigned int>& Indices)
{
	vector<Face> uniqueFaces;
	map<Edge, Neighbors> indexMap;
	int index = 0;
	for (auto &i : mesh)
	{
		Edge e1(i.indices[0], i.indices[1]);
		Edge e2(i.indices[1], i.indices[2]);
		Edge e3(i.indices[2], i.indices[0]);
		//neighbor 共享一条边的三角形
		indexMap[e1].AddNeigbor(index);
		indexMap[e2].AddNeigbor(index);
		indexMap[e3].AddNeigbor(index);
		++index;
	}
	index = 0;
	for (auto &i : mesh)
	{
		for (int j = 0; j < 3; ++j)
		{
			Edge e(i.indices[j], i.indices[(j + 1) % 3]);
			Neighbors n = indexMap[e];
			GLuint otherTri = n.GetOther(index);

			Indices.push_back(i.indices[j]);
			Indices.push_back(mesh[otherTri].GetOppositeIndex(e));

		}
		++index;
	}
}