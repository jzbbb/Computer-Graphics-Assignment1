/*
Type your name and student ID here
	- Name:
	- Student ID:
*/

#include "Dependencies/glew/glew.h"
#include "Dependencies/GLFW/glfw3.h"

#include "Dependencies/glm/glm.hpp"
#include "Dependencies/glm/gtc/matrix_transform.hpp"

#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

GLint programID;

unsigned int VBO[21], VAO[21], EBO[16];
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1600;
float x_delta = 0.1f;		//一次移动距离       
float x_press_num = 0;		//x方向位移   
float y_press_num = 0;		//y方向位移
float z_press_num = 0;		//z方向位移

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

glm::vec3 cubePositions[] = {
	glm::vec3(-2.5, -2.5, 0.0f),
	glm::vec3(2.5,  2.0, 0.2f)
};

glm::vec3 body_color;
int body_color_index = 0;

glm::vec3 colorPositions[] = {
	glm::vec3(0.01f, 0.01f, 0.01f),
	glm::vec3(1.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 1.0f),
	glm::vec3(1.0f, 1.0f, 0.0f),
	glm::vec3(1.0f, 0.0f, 1.0f),
	glm::vec3(0.0f, 1.0f, 1.0f),
};

glm::vec3 riverPositions[] = {
	glm::vec3(2.0f, -0.4f, 0.0f),
	glm::vec3(1.0f, -0.45f, 0.0f),
	glm::vec3(-2.0f, -0.3f, 0.0f),
	glm::vec3(-1.0f, -0.45f, 0.0f),
};

float fish_x = 0.0f;		//鱼的横坐标
float angel = 0.0f;

float aspect = 2.0f;


const GLfloat  PI = 3.14159265358979323846f;

//将球横纵划分成50X50的网格
const int Y_SEGMENTS = 100;
const int X_SEGMENTS = 100;

void get_OpenGL_info() {
	// OpenGL information
	const GLubyte* name = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* glversion = glGetString(GL_VERSION);
	std::cout << "OpenGL company: " << name << std::endl;
	std::cout << "Renderer name: " << renderer << std::endl;
	std::cout << "OpenGL version: " << glversion << std::endl;
}

bool checkStatus(
	GLuint objectID,
	PFNGLGETSHADERIVPROC objectPropertyGetterFunc,
	PFNGLGETSHADERINFOLOGPROC getInfoLogFunc,
	GLenum statusType)
{
	GLint status;
	objectPropertyGetterFunc(objectID, statusType, &status);
	if (status != GL_TRUE)
	{
		GLint infoLogLength;
		objectPropertyGetterFunc(objectID, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* buffer = new GLchar[infoLogLength];

		GLsizei bufferSize;
		getInfoLogFunc(objectID, infoLogLength, &bufferSize, buffer);
		std::cout << buffer << std::endl;

		delete[] buffer;
		return false;
	}
	return true;
}

bool checkShaderStatus(GLuint shaderID) {
	return checkStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}

bool checkProgramStatus(GLuint programID) {
	return checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}

std::string readShaderCode(const char* fileName) {
	std::ifstream meInput(fileName);
	if (!meInput.good()) {
		std::cout << "File failed to load ... " << fileName << std::endl;
		exit(1);
	}
	return std::string(
		std::istreambuf_iterator<char>(meInput),
		std::istreambuf_iterator<char>()
	);
}

void installShaders() {
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar* adapter[1];
	//adapter[0] = vertexShaderCode;
	std::string temp = readShaderCode("VertexShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	//adapter[0] = fragmentShaderCode;
	temp = readShaderCode("FragmentShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	if (!checkShaderStatus(vertexShaderID) || !checkShaderStatus(fragmentShaderID))
		return;

	programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	if (!checkProgramStatus(programID))
		return;

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	glUseProgram(programID);

}

void sendDataToOpenGL() {
	// TODO:
	// create 2D objects and 3D objects and/or lines (points) here and bind to VAOs & VBOs

	float Triangles[] = {
	-0.5f, -0.5f, -0.5f,  1.0f, 0.0f,0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 0.0f,0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 0.0f,0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 0.0f,0.0f,
	-0.5f, -0.5f, -0.5f,  1.0f, 0.0f,0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 1.0f,0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 1.0f,0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f, 1.0f,0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f, 1.0f,0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 1.0f,0.0f,

	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 0.0f,1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,1.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 1.0f,0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,0.0f,

	-0.5f, -0.5f, -0.5f,  1.0f, 0.0f,1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,1.0f,
	-0.5f, -0.5f,  0.5f,  1.0f, 0.0f,1.0f,
	-0.5f, -0.5f, -0.5f,  1.0f, 0.0f,1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f, 1.0f,1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f, 1.0f,1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f, 1.0f,1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,1.0f
	};

	float Triangles_Lines[] = {
		0.5f, 0.5f, 0.5f, 1.0f,0.0f,0.0f,
	   -0.5f, 0.5f, 0.5f, 0.0f,1.0f,0.0f,
		0.5f,-0.5f, 0.5f, 0.0f,0.0f,1.0f,
		0.5f, 0.5f,-0.5f, 1.0f,1.0f,0.0f,
	   -0.5f,-0.5f, 0.5f, 1.0f,0.0f,1.0f,
	   -0.5f, 0.5f,-0.5f, 0.0f,1.0f,1.0f,
		0.5f,-0.5f,-0.5f, 1.0f,1.0f,1.0f,
	   -0.5f,-0.5f,-0.5f, 0.5f,0.5f,0.5f
	};

	unsigned int Triangles_indices[] = {
		0,1,
		0,2,
		1,4,
		2,4,
		1,5,
		4,7,
		2,6,
		0,3,
		5,7,
		6,7,
		3,6,
		3,5
	};

	float Quad[] = {
		0.6f,  0.5f, 0.0f,   0.8f,0.8f,0.8f,
		0.6f, -0.5f, 0.0f,   0.8f,0.8f,0.8f,
	   -0.6f, -0.5f, 0.0f,   0.8f,0.8f,0.8f,
	   -0.6f,  0.5f, 0.0f,   0.8f,0.8f,0.8f
	};
	unsigned int Quad_indices[] = {
		0, 1, 3,
		1, 2, 3,
	};
	float Quad_Lines[] = {
		0.6f,  0.5f, 0.0f,   
		0.6f, -0.5f, 0.0f,   
	   -0.6f, -0.5f, 0.0f,   
	   -0.6f,  0.5f, 0.0f   
	};

	float Tree[] = {
		0.25f,  0.25f, 0.0f, 0.8f,0.3f,0.2f,
	   -0.25f,  0.25f, 0.0f, 0.8f,0.3f,0.2f,
		0.25f, -0.25f, 0.0f, 0.8f,0.3f,0.2f,
	   -0.25f, -0.25f, 0.0f, 0.8f,0.3f,0.2f,
	    0.25f,  0.25f, 0.9f, 0.8f,0.3f,0.2f,
	   -0.25f,  0.25f, 0.9f, 0.8f,0.3f,0.2f,
		0.25f, -0.25f, 0.9f, 0.8f,0.3f,0.2f,
	   -0.25f, -0.25f, 0.9f, 0.8f,0.3f,0.2f
	};
	float Tree_Lines[] = {
		0.25f,  0.25f, 0.0f, 1.0f,1.0f,1.0f,  
		0.25f,  0.25f, 0.9f, 1.0f,1.0f,1.0f,  
	   -0.25f,  0.25f, 0.0f, 1.0f,1.0f,1.0f,  
	   -0.25f,  0.25f, 0.9f, 1.0f,1.0f,1.0f,  
		0.25f, -0.25f, 0.0f, 1.0f,1.0f,1.0f,  
		0.25f, -0.25f, 0.9f, 1.0f,1.0f,1.0f,  
	   -0.25f, -0.25f, 0.0f, 1.0f,1.0f,1.0f,  
	   -0.25f, -0.25f, 0.9f, 1.0f,1.0f,1.0f  
	};

	unsigned int Tree_indices[] = {
		0,1,2,
		1,2,3,
		0,1,5,
		0,4,5,
		0,2,6,
		0,4,6,
		2,3,7,
		2,7,6,
		1,3,7,
		1,5,7,
	};

	float Triangles1[] = {
		-0.6f, 0.5f, 0.9f, 
		 0.6f, 0.5f, 0.9f, 
		 0.6f,-0.5f, 0.9f, 
		-0.6f,-0.5f, 0.9f, 
		 0.0f, 0.0f, 1.4f, 

	};

	unsigned int Triangles1_indices[] = {
		0,1,2,
		0,2,3,
		0,1,4,
		0,3,4,
		2,3,4,
		1,2,4
	};

	float Triangles1_Lines[] = {
		-0.6f, 0.5f, 0.9f, 1.0f,1.0f,1.0f,
		 0.6f, 0.5f, 0.9f, 1.0f,1.0f,1.0f,
		 0.6f,-0.5f, 0.9f, 1.0f,1.0f,1.0f,
		-0.6f,-0.5f, 0.9f, 1.0f,1.0f,1.0f,
		 0.0f, 0.0f, 1.4f, 1.0f,1.0f,1.0f
	};
	unsigned int Triangles1_Lines_indices[] = {
		1,2,
		2,3,
		3,0,
		0,1,
		1,4,
		2,4,
		3,4,
		0,4
	};

	float Triangles2[] = {
	-0.6f, 0.5f, 0.5f, 1.0f,1.0f,0.0f,
	 0.6f, 0.5f, 0.5f, 0.0f,1.0f,1.0f,
	 0.0f,-0.5f, 0.5f, 1.0f,1.0f,1.0f,
	 0.0f, 0.0f, 1.0f, 0.0f,1.0f,0.0f
	};
	unsigned int Triangles2_indices[] = {
		0,1,3,
		0,1,2,
		0,2,3,
		1,2,3
	};

	float Triangles2_Lines[] = {
		-0.6f, 0.5f, 0.5f, 1.0f,1.0f,1.0f,
		 0.6f, 0.5f, 0.5f, 1.0f,1.0f,1.0f,
		 0.0f,-0.5f, 0.5f, 1.0f,1.0f,1.0f,
		 0.0f, 0.0f, 1.0f, 1.0f,1.0f,1.0f
	};
	unsigned int Triangles2_Lines_indices[] = {
		0,1,
		0,2,
		0,3,
		1,2,
		1,3,
		2,3
	};

	float Person_Head[] = {
		//head
		0.2f,  0.2f, 1.3f, 1.0f,1.0f,1.0f,
	   -0.2f,  0.2f, 1.3f, 1.0f,1.0f,1.0f,
		0.2f, -0.2f, 1.3f, 1.0f,1.0f,1.0f,
	   -0.2f, -0.2f, 1.3f, 1.0f,1.0f,1.0f,
		0.2f,  0.2f, 1.6f, 1.0f,1.0f,1.0f,
	   -0.2f,  0.2f, 1.6f, 1.0f,1.0f,1.0f,
		0.2f, -0.2f, 1.6f, 1.0f,1.0f,1.0f,
	   -0.2f, -0.2f, 1.6f, 1.0f,1.0f,1.0f,
	};
	unsigned int Person_Head_indices[] = {
		0,1,2,
		1,2,3,
		0,1,5,
		0,4,5,
		0,2,6,
		0,4,6,
		2,3,7,
		2,7,6,
		1,3,7,
		1,5,7,
		4,5,7,
		4,6,7
	};
	float Person_Body[] = {
		//body
		0.5f,  0.5f, 0.8f, 0.01f,0.0f,0.0f,
	   -0.5f,  0.5f, 0.8f, 0.01f,0.0f,0.0f,
		0.5f, -0.5f, 0.8f, 0.01f,0.0f,0.0f,
	   -0.5f, -0.5f, 0.8f, 0.01f,0.0f,0.0f,
		0.5f,  0.5f, 1.6f, 0.01f,0.0f,0.0f,
	   -0.5f,  0.5f, 1.6f, 0.01f,0.0f,0.0f,
		0.5f, -0.5f, 1.6f, 0.01f,0.0f,0.0f,
	   -0.5f, -0.5f, 1.6f, 0.01f,0.0f,0.0f,
	};
	unsigned int Person_Body_indices[] = {
		0,1,2,
		1,2,3,
		0,1,5,
		0,4,5,
		0,2,6,
		0,4,6,
		2,3,7,
		2,7,6,
		1,3,7,
		1,5,7,
		4,5,7,
		4,6,7
	};
	float Person_Hand_Leg[] = {
		//hand
		0.5f, 0.0f, 1.4f, 1.0f,0.9f,0.8f,
		1.0f, 0.0f, 1.0f, 1.0f,0.9f,0.8f,
	   -0.5f, 0.0f, 1.4f, 1.0f,0.9f,0.8f,
	   -1.0f, 0.0f, 1.0f, 1.0f,0.9f,0.8f,
	   //leg
		0.25f, 0.0f, 0.8f, 1.0f,0.9f,0.8f,
		0.25f, 0.0f, 0.0f, 1.0f,0.9f,0.8f,
	   -0.25f, 0.0f, 0.8f, 1.0f,0.9f,0.8f,
	   -0.25f, 0.0f, 0.0f, 1.0f,0.9f,0.8f,
	};

	float Mountain[] = {
	-1.0f, -1.0f,  0.0f,  0.9f, 0.7f,0.5f,//bottom
	 1.0f, -1.0f, -0.0f,  0.9f, 0.7f,0.5f,
	 1.0f,  1.0f, -0.0f,  0.9f, 0.7f,0.5f,
	-1.0f,  1.0f, -0.0f,  0.9f, 0.7f,0.5f,

	-0.5f, -0.5f,  1.0f,  0.5f, 1.0f,0.5f,//top
	 0.5f, -0.5f,  1.0f,  0.5f, 1.0f,0.5f,
	 0.5f,  0.5f,  1.0f,  0.5f, 1.0f,0.5f,
	-0.5f,  0.5f,  1.0f,  0.5f, 1.0f,0.5f,

	-0.75f, -0.75f,  0.5f,  0.9f, 0.7f,0.5f,//top
	 0.75f, -0.75f,  0.5f,  0.9f, 0.7f,0.5f,
	 0.75f,  0.75f,  0.5f,  0.9f, 0.7f,0.5f,
	-0.75f,  0.75f,  0.5f,  0.9f, 0.7f,0.5f,
	};

	unsigned int Mountain_indices[] = {
		0,1,3,
		1,2,3,
		2,10,11,
		2,11,3,
		2,10,1,
		1,10,9,
		1,8,9,
		1,0,9,
		3,0,8,
		3,11,8,
		7,11,6,
		10,11,6,
		6,10,5,
		5,10,9,
		4,5,9,
		9,4,8,
		4,7,11,
		4,8,11,
		4,6,7,
		4,5,6
	};

	float House[] = {
		0.4f, 0.4f, 0.6f, 1.0f, 0.8f, 0.6f,
		0.4f,-0.4f, 0.6f, 1.0f, 0.8f, 0.6f,
	   -0.4f,-0.4f, 0.6f, 1.0f, 0.8f, 0.6f,
	   -0.4f, 0.4f, 0.6f, 1.0f, 0.8f, 0.6f,
	    0.0f, 0.4f, 1.0f, 1.0f, 0.8f, 0.6f,
		0.0f,-0.4f, 1.0f, 1.0f, 0.8f, 0.6f,

		0.4f, 0.4f, 0.6f, 0.8f, 0.3f, 0.1f,
		0.4f,-0.4f, 0.6f, 0.8f, 0.3f, 0.1f,
	   -0.4f,-0.4f, 0.6f, 0.8f, 0.3f, 0.1f,
	   -0.4f, 0.4f, 0.6f, 0.8f, 0.3f, 0.1f,
		0.0f, 0.4f, 1.0f, 0.8f, 0.3f, 0.1f,
		0.0f,-0.4f, 1.0f, 0.8f, 0.3f, 0.1f,

		0.3f, 0.3f, 0.0f, 0.9f, 0.8f, 0.8f,
		0.3f,-0.3f, 0.0f, 0.9f, 0.8f, 0.8f,
	   -0.3f,-0.3f, 0.0f, 0.9f, 0.8f, 0.8f,
	   -0.3f, 0.3f, 0.0f, 0.9f, 0.8f, 0.8f,
	    0.3f, 0.3f, 0.6f, 0.9f, 0.8f, 0.8f,
		0.3f,-0.3f, 0.6f, 0.9f, 0.8f, 0.8f,
	   -0.3f,-0.3f, 0.6f, 0.9f, 0.8f, 0.8f,
	   -0.3f, 0.3f, 0.6f, 0.9f, 0.8f, 0.8f,

	    0.0f, 0.31f, 0.0f, 1.0f, 0.4f, 0.4f,
		0.2f, 0.31f, 0.0f, 1.0f, 0.4f, 0.4f,
		0.0f, 0.31f, 0.4f, 1.0f, 0.4f, 0.4f,
		0.2f, 0.31f, 0.4f, 1.0f, 0.4f, 0.4f,

		-0.1f, 0.31f, 0.4f, 0.5f, 0.8f, 1.0f,
		-0.2f, 0.31f, 0.4f, 0.5f, 0.8f, 1.0f,
		-0.1f, 0.31f, 0.5f, 0.5f, 0.8f, 1.0f,
		-0.2f, 0.31f, 0.5f, 0.5f, 0.8f, 1.0f,
	};


	unsigned int House_indices[] = {
		0,3,4,
		2,1,5,
		6,10,7,
		7,10,11,
		8,9,10,
		8,10,11,
		12,13,15,
		13,15,14,
		12,15,19,
		12,19,16,
		12,13,16,
		13,16,17,
		13,14,18,
		13,17,18,
		14,15,18,
		15,19,18,
		20,21,22,
		21,22,23,
		24,25,26,
		25,26,27
	};
	
	float House_Lines[] = {
		0.4f, 0.4f, 0.6f, 1.0f, 1.0f, 1.0f,
		0.4f,-0.4f, 0.6f, 1.0f, 1.0f, 1.0f,
	   -0.4f,-0.4f, 0.6f, 1.0f, 1.0f, 1.0f,
	   -0.4f, 0.4f, 0.6f, 1.0f, 1.0f, 1.0f,
		0.0f, 0.4f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.0f,-0.4f, 1.0f, 1.0f, 1.0f, 1.0f,

		0.3f, 0.3f, 0.0f, 1.0f, 1.0f, 1.0f,
		0.3f,-0.3f, 0.0f, 1.0f, 1.0f, 1.0f,
	   -0.3f,-0.3f, 0.0f, 1.0f, 1.0f, 1.0f,
	   -0.3f, 0.3f, 0.0f, 1.0f, 1.0f, 1.0f,
		0.3f, 0.3f, 0.6f, 1.0f, 1.0f, 1.0f,
		0.3f,-0.3f, 0.6f, 1.0f, 1.0f, 1.0f,
	   -0.3f,-0.3f, 0.6f, 1.0f, 1.0f, 1.0f,
	   -0.3f, 0.3f, 0.6f, 1.0f, 1.0f, 1.0f,

	    0.0f, 0.31f, 0.0f, 1.0f, 1.0f, 1.0f,
		0.2f, 0.31f, 0.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 0.31f, 0.4f, 1.0f, 1.0f, 1.0f,
		0.2f, 0.31f, 0.4f, 1.0f, 1.0f, 1.0f,

		-0.1f, 0.31f, 0.4f, 1.0f, 1.0f, 1.0f,
		-0.2f, 0.31f, 0.4f, 1.0f, 1.0f, 1.0f,
		-0.1f, 0.31f, 0.5f, 1.0f, 1.0f, 1.0f,
		-0.2f, 0.31f, 0.5f, 1.0f, 1.0f, 1.0f,

		-0.15f, 0.31f, 0.4f, 1.0f, 1.0f, 1.0f,
		-0.2f, 0.31f, 0.45f, 1.0f, 1.0f, 1.0f,
		-0.1f, 0.31f, 0.45f, 1.0f, 1.0f, 1.0f,
		-0.15f, 0.31f, 0.5f, 1.0f, 1.0f, 1.0f,
	};
	unsigned int House_Lines_indices[] = {
		0,3,
		0,4,
		3,4,
		0,1,
		1,5,
		5,4,
		1,2,
		2,5,
		2,3,
		6,9,
		6,7,
		7,8,
		8,9,
		6,10,
		7,11,
		8,12,
		9,13,
		10,13,
		10,11,
		11,12,
		12,13,
		14,16,
		16,17,
		15,17,
		18,20,
		20,21,
		21,19,
		19,18,
		22,25,
		23,24
	};
	
	float River[] = {
		-0.6f, -0.15f, 0.0f, 0.5f,0.8f,0.9f,
		 0.6f, -0.15f, 0.0f, 0.5f,0.8f,0.9f,
		 0.6f,  0.05f, 0.0f, 0.5f,0.8f,0.9f,
		-0.6f,  0.05f, 0.0f, 0.5f,0.8f,0.9f,
	};
	unsigned int River_indices[] = {
		0,1,2,
		0,2,3
	};

	float Fish[] = {
		0.5f,0.0f,0.1f,  1.0f,0.8f,0.0f,
		0.0f,0.4f,0.1f,	 1.0f,0.8f,0.0f,
		0.0f,-0.4f,0.1f, 1.0f,0.8f,0.0f,
		-0.5f,-0.0f,0.1f,1.0f,0.8f,0.0f,
		-0.7f,0.2f,0.1f, 1.0f,0.8f,0.7f,
		-0.7f,-0.2f,0.1f,1.0f,0.8f,0.7f,
	};

	unsigned int Fish_indices[] = {
		0,1,2,
		1,2,3,
		3,4,5
	};
	
	std::vector<float> sphereVertices;
	std::vector<float> sphereVertices2;
	std::vector<int> sphereIndices;


	// 生成球的顶点
	for (int y = 0; y <= Y_SEGMENTS; y++)
	{
		for (int x = 0; x <= X_SEGMENTS; x++)
		{
			float xSegment = (float)x / (float)X_SEGMENTS;
			float ySegment = (float)y / (float)Y_SEGMENTS;
			float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
			float yPos = std::cos(ySegment * PI);
			float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);


			sphereVertices.push_back(xPos);
			sphereVertices.push_back(yPos);
			sphereVertices.push_back(zPos+5.0);
			sphereVertices.push_back(1.0);
			sphereVertices.push_back(1.0);
			sphereVertices.push_back(1.0);
			sphereVertices2.push_back(xPos);
			sphereVertices2.push_back(yPos);
			sphereVertices2.push_back(zPos + 1.0);
		}
	}

	// 生成球的Indices
	for (int i = 0; i < Y_SEGMENTS; i++)
	{
		for (int j = 0; j < X_SEGMENTS; j++)
		{

			sphereIndices.push_back(i * (X_SEGMENTS + 1) + j);
			sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j);
			sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1);

			sphereIndices.push_back(i * (X_SEGMENTS + 1) + j);
			sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1);
			sphereIndices.push_back(i * (X_SEGMENTS + 1) + j + 1);
		}
	}

	std::vector<float> sinVertices;
	for (float x = -PI; x < PI; x += 2 * PI / 180) {
		float y = sin(x);
		sinVertices.push_back(x);
		sinVertices.push_back(y);
		sinVertices.push_back(0.1);
		sinVertices.push_back(1);
		sinVertices.push_back(1);
		sinVertices.push_back(1);
	}
	
	glGenVertexArrays(21, VAO);
	glGenBuffers(21, VBO);
	glGenBuffers(16, EBO);
	

	// Triangles
	// ---------------------
	glBindVertexArray(VAO[0]);	// note that we bind to a different VAO now
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);	// and a different VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(Triangles), Triangles, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // because the vertex data is tightly packed we can also specify 0 as the vertex attribute's stride to let OpenGL figure it out
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));

	//Triangles_Lines
	// ---------------------
	glBindVertexArray(VAO[1]);	// note that we bind to a different VAO now
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);	// and a different VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(Triangles_Lines), Triangles_Lines, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // because the vertex data is tightly packed we can also specify 0 as the vertex attribute's stride to let OpenGL figure it out
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Triangles_indices), Triangles_indices, GL_STATIC_DRAW);

	//Quad
	// ---------------------
	glBindVertexArray(VAO[2]);	// note that we bind to a different VAO now
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);	// and a different VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(Quad), Quad, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // because the vertex data is tightly packed we can also specify 0 as the vertex attribute's stride to let OpenGL figure it out
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Quad_indices), Quad_indices, GL_STATIC_DRAW);

	//Quad_Lines
	// ---------------------
	glBindVertexArray(VAO[3]);	// note that we bind to a different VAO now
	glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);	// and a different VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(Quad_Lines), Quad_Lines, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // because the vertex data is tightly packed we can also specify 0 as the vertex attribute's stride to let OpenGL figure it out
	glEnableVertexAttribArray(0);

	//Tree
	// ---------------------
	glBindVertexArray(VAO[8]);	// note that we bind to a different VAO now
	glBindBuffer(GL_ARRAY_BUFFER, VBO[8]);	// and a different VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(Tree), Tree, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // because the vertex data is tightly packed we can also specify 0 as the vertex attribute's stride to let OpenGL figure it out
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[6]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Tree_indices), Tree_indices, GL_STATIC_DRAW);

	//Tree_Lines
	// ---------------------
	glBindVertexArray(VAO[9]);	// note that we bind to a different VAO now
	glBindBuffer(GL_ARRAY_BUFFER, VBO[9]);	// and a different VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(Tree_Lines), Tree_Lines, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // because the vertex data is tightly packed we can also specify 0 as the vertex attribute's stride to let OpenGL figure it out
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));

	//Triangles1
	// ---------------------
	glBindVertexArray(VAO[4]);	// note that we bind to a different VAO now
	glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);	// and a different VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(Triangles1), Triangles1, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // because the vertex data is tightly packed we can also specify 0 as the vertex attribute's stride to let OpenGL figure it out
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Triangles1_indices), Triangles1_indices, GL_STATIC_DRAW);

	//Triangles1_Lines
	// ---------------------
	glBindVertexArray(VAO[5]);	// note that we bind to a different VAO now
	glBindBuffer(GL_ARRAY_BUFFER, VBO[5]);	// and a different VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(Triangles1_Lines), Triangles1_Lines, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // because the vertex data is tightly packed we can also specify 0 as the vertex attribute's stride to let OpenGL figure it out
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Triangles1_Lines_indices), Triangles1_Lines_indices, GL_STATIC_DRAW);

	//Triangles2
	// ---------------------
	glBindVertexArray(VAO[6]);	// note that we bind to a different VAO now
	glBindBuffer(GL_ARRAY_BUFFER, VBO[6]);	// and a different VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(Triangles2), Triangles2, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // because the vertex data is tightly packed we can also specify 0 as the vertex attribute's stride to let OpenGL figure it out
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[4]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Triangles2_indices), Triangles2_indices, GL_STATIC_DRAW);

	//Triangles2_Lines
	// ---------------------
	glBindVertexArray(VAO[7]);	// note that we bind to a different VAO now
	glBindBuffer(GL_ARRAY_BUFFER, VBO[7]);	// and a different VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(Triangles2_Lines), Triangles2_Lines, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // because the vertex data is tightly packed we can also specify 0 as the vertex attribute's stride to let OpenGL figure it out
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[5]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Triangles2_Lines_indices), Triangles2_Lines_indices, GL_STATIC_DRAW);

	//Person_Head
	// ---------------------
	glBindVertexArray(VAO[10]);	// note that we bind to a different VAO now
	glBindBuffer(GL_ARRAY_BUFFER, VBO[10]);	// and a different VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(Person_Head), Person_Head, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // because the vertex data is tightly packed we can also specify 0 as the vertex attribute's stride to let OpenGL figure it out
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[7]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Person_Head_indices), Person_Head_indices, GL_STATIC_DRAW);

	//Person_Body
	// ---------------------
	glBindVertexArray(VAO[11]);	// note that we bind to a different VAO now
	glBindBuffer(GL_ARRAY_BUFFER, VBO[11]);	// and a different VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(Person_Body), Person_Body, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // because the vertex data is tightly packed we can also specify 0 as the vertex attribute's stride to let OpenGL figure it out
	glEnableVertexAttribArray(0);
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[8]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Person_Body_indices), Person_Body_indices, GL_STATIC_DRAW);

	//Person_Hand_Leg
	// ---------------------
	glBindVertexArray(VAO[12]);	// note that we bind to a different VAO now
	glBindBuffer(GL_ARRAY_BUFFER, VBO[12]);	// and a different VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(Person_Hand_Leg), Person_Hand_Leg, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // because the vertex data is tightly packed we can also specify 0 as the vertex attribute's stride to let OpenGL figure it out
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));

	//Person_Head
	// ---------------------
	glBindVertexArray(VAO[13]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[13]);
	glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), &sphereVertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // because the vertex data is tightly packed we can also specify 0 as the vertex attribute's stride to let OpenGL figure it out
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[9]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(int), &sphereIndices[0], GL_STATIC_DRAW);

	//Gate
	// ---------------------
	glBindVertexArray(VAO[14]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[14]);
	glBufferData(GL_ARRAY_BUFFER, sphereVertices2.size() * sizeof(float), &sphereVertices2[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // because the vertex data is tightly packed we can also specify 0 as the vertex attribute's stride to let OpenGL figure it out
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[10]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(int), &sphereIndices[0], GL_STATIC_DRAW);

	//Mountain
	// ---------------------
	glBindVertexArray(VAO[15]);	// note that we bind to a different VAO now
	glBindBuffer(GL_ARRAY_BUFFER, VBO[15]);	// and a different VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(Mountain), Mountain, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // because the vertex data is tightly packed we can also specify 0 as the vertex attribute's stride to let OpenGL figure it out
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[11]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Mountain_indices), Mountain_indices, GL_STATIC_DRAW);

	//House
	// ---------------------
	glBindVertexArray(VAO[16]);	// note that we bind to a different VAO now
	glBindBuffer(GL_ARRAY_BUFFER, VBO[16]);	// and a different VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(House), House, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // because the vertex data is tightly packed we can also specify 0 as the vertex attribute's stride to let OpenGL figure it out
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[12]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(House_indices), House_indices, GL_STATIC_DRAW);

	//House_Lines
	// ---------------------
	glBindVertexArray(VAO[17]);	// note that we bind to a different VAO now
	glBindBuffer(GL_ARRAY_BUFFER, VBO[17]);	// and a different VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(House_Lines), House_Lines, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // because the vertex data is tightly packed we can also specify 0 as the vertex attribute's stride to let OpenGL figure it out
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[13]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(House_Lines_indices), House_Lines_indices, GL_STATIC_DRAW);

	//River
	// ---------------------
	glBindVertexArray(VAO[18]);	// note that we bind to a different VAO now
	glBindBuffer(GL_ARRAY_BUFFER, VBO[18]);	// and a different VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(River), River, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // because the vertex data is tightly packed we can also specify 0 as the vertex attribute's stride to let OpenGL figure it out
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[14]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(River_indices), River_indices, GL_STATIC_DRAW);

	//sin
	// ---------------------
	glBindVertexArray(VAO[19]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[19]);
	glBufferData(GL_ARRAY_BUFFER, sinVertices.size() * sizeof(float), &sinVertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // because the vertex data is tightly packed we can also specify 0 as the vertex attribute's stride to let OpenGL figure it out
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));
	
	//Fish
	// ---------------------
	glBindVertexArray(VAO[20]);	// note that we bind to a different VAO now
	glBindBuffer(GL_ARRAY_BUFFER, VBO[20]);	// and a different VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(Fish), Fish, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // because the vertex data is tightly packed we can also specify 0 as the vertex attribute's stride to let OpenGL figure it out
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char*)(3 * sizeof(float)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[15]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Fish_indices), Fish_indices, GL_STATIC_DRAW);
}

void paintGL(void) {
	// always run
	// TODO:
	// render your objects and control the transformation here
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);

	projection = glm::perspective(glm::radians(fov), aspect, 0.1f, 20.0f);
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
	glUniformMatrix4fv(glGetUniformLocation(programID, "projection"), 1, GL_FALSE, &projection[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(programID, "view"), 1, GL_FALSE, &view[0][0]);
	

	glLineWidth(4.0f);
	glDisable(GL_LINE_STIPPLE);
	for (int i = 0; i < 9; i++) {
		if (i == 0) {
			model = glm::mat4(1.0f);	
			//model = glm::rotate(model, float(glfwGetTime()) * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::translate(model, glm::vec3(4.0f, 0.0f, 0.0f));
			model = glm::rotate(model, float(glfwGetTime()) * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.3f));
			model = glm::scale(model, glm::vec3(0.5f));
			glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, &model[0][0]);
			
			glBindVertexArray(VAO[0]);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(VAO[1]);
			glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
		}
		else if (i == 1) {
			model = glm::mat4(1.0f);
			model = glm::rotate(model, glm::radians(-50.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(5.0f));
			glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, &model[0][0]);
			glDisable(GL_LINE_STIPPLE);
			glBindVertexArray(VAO[2]);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);		
			glBindVertexArray(VAO[3]);
			glUniform3f(glGetUniformLocation(programID, "incolor"), cos(glfwGetTime())/2.0f+0.5f, sin(glfwGetTime())/2.0f+0.5f, cos(glfwGetTime()));
			glDrawArrays(GL_LINE_LOOP, 0, 4);
		}
		else if (i == 2) {
			glLineWidth(3.0f);
			model = glm::mat4(1.0f);
			model = glm::rotate(model, glm::radians(-50.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.5f));
			model = glm::translate(model, glm::vec3(-5.0f, 2.0f, 0.0f));
			glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, &model[0][0]);
			glBindVertexArray(VAO[4]);
			glUniform3f(glGetUniformLocation(programID, "incolor"), 0.0f, sin(glfwGetTime()) / 2.0f + 1.0f, 0.0f);
			glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
			glBindVertexArray(VAO[5]);
			glDrawElements(GL_LINE_LOOP, 16, GL_UNSIGNED_INT, 0);
			glBindVertexArray(VAO[8]);
			glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_INT, 0);
			glBindVertexArray(VAO[9]);
			glDrawArrays(GL_LINES, 0, 8);
			model = glm::translate(model, glm::vec3(0.5f, 2.0f, 0.0f));
			model = glm::scale(model, glm::vec3(2.0f));			
			glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, &model[0][0]);
			glBindVertexArray(VAO[4]);
			glUniform3f(glGetUniformLocation(programID, "incolor"), 0.0f, sin(glfwGetTime()) / 2.0f + 1.0f, 0.0f);
			glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
			glBindVertexArray(VAO[5]);
			glDrawElements(GL_LINE_LOOP, 16, GL_UNSIGNED_INT, 0);
			glBindVertexArray(VAO[8]);
			glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_INT, 0);
			glBindVertexArray(VAO[9]);
			glDrawArrays(GL_LINES, 0, 8);
		}
		else if (i == 3) {//person
			model = glm::mat4(1.0f);
			model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::translate(model, glm::vec3(x_press_num, y_press_num, z_press_num));
			model = glm::scale(model, glm::vec3(0.5f));
			
			glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, &model[0][0]);
			//glBindVertexArray(VAO[10]);
			//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			glUniform3f(glGetUniformLocation(programID, "incolor"), body_color.x, body_color.y, body_color.z);
			glBindVertexArray(VAO[11]);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			glLineWidth(15.0f);
			glBindVertexArray(VAO[12]);
			glDrawArrays(GL_LINES, 0, 8);
		}
		else if (i == 4) {
			model = glm::mat4(1.0f);			
			model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));	
			model = glm::translate(model, glm::vec3(x_press_num, y_press_num, z_press_num));
			model = glm::scale(model, glm::vec3(0.2f));			
			
			glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, &model[0][0]);
			glBindVertexArray(VAO[13]);
			//使用线框模式绘制
			glDrawElements(GL_TRIANGLES, X_SEGMENTS * Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);
		}
		else if (i == 5) {
			for (int j = 0; j < 2; j++) {
				model = glm::mat4(1.0f);
				model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::translate(model, cubePositions[j]);
				model = glm::scale(model, glm::vec3(0.2f));
				glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, &model[0][0]);
				glUniform3f(glGetUniformLocation(programID, "incolor"), 0.2f,0.2f , sin(glfwGetTime()) / 2.0f + 0.5f);
				glBindVertexArray(VAO[14]);
				glDrawElements(GL_TRIANGLES, X_SEGMENTS * Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);
			}				
		}
		else if (i == 6) {
			model = glm::mat4(1.0f);
			model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::translate(model, glm::vec3(2.0, -1.7, 0.0f));
			model = glm::scale(model, glm::vec3(0.8f));
			glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, &model[0][0]);
			glBindVertexArray(VAO[15]);
			glDrawElements(GL_TRIANGLES, 60, GL_UNSIGNED_INT, 0);
			model = glm::mat4(1.0f);
			model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::translate(model, glm::vec3(-1.0, -1.5, 0.0f));
			model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::scale(model, glm::vec3(0.5f));
			glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, &model[0][0]);
			glBindVertexArray(VAO[15]);
			glDrawElements(GL_TRIANGLES, 60, GL_UNSIGNED_INT, 0);
		}
		else if (i == 7) {//House
			model = glm::mat4(1.0f);
			model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(-150.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::translate(model, glm::vec3(-0.5, -2.0, 0.0f));
			model = glm::scale(model, glm::vec3(1.5f));
			glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, &model[0][0]);
			glBindVertexArray(VAO[16]);
			glDrawElements(GL_TRIANGLES, 60, GL_UNSIGNED_INT, 0);
			glLineWidth(4.0f);
			glBindVertexArray(VAO[17]);
			glDrawElements(GL_LINES, 60, GL_UNSIGNED_INT, 0);
		}
		else if (i == 8) {
			model = glm::mat4(1.0f);
			model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(5.0f));
			glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, &model[0][0]);
			glBindVertexArray(VAO[18]);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			for (int j = 0; j < 4; j++) {
				model = glm::mat4(1.0f);
				model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::translate(model, riverPositions[j]);
				model = glm::scale(model, glm::vec3(0.2f));
				glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, &model[0][0]);
				glBindVertexArray(VAO[19]);
				glDrawArrays(GL_LINE_STRIP, 0, 179);
			}
			model = glm::mat4(1.0f);
			model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::translate(model, glm::vec3(fish_x,-0.5f,0.0f));
			model = glm::scale(model, glm::vec3(0.5f));
			model = glm::rotate(model, glm::radians(angel), glm::vec3(0.0f, 1.0f, 0.0f));
			glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, &model[0][0]);
			glBindVertexArray(VAO[20]);
			glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
		}
}
	


}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// TODO:
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	float currentFrame = static_cast<float>(glfwGetTime());
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	float cameraSpeed = static_cast<float>(2.5 * deltaTime);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cameraPos += cameraSpeed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		cameraPos -= cameraSpeed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		y_press_num += 0.5;
		if (y_press_num == 2 && x_press_num == 2.5) {
			y_press_num = -2.5;
			x_press_num = -2.5;
		}
		if (y_press_num == 2.5) {
			y_press_num = -2.5;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		y_press_num -= 0.5;
		if (y_press_num == -2.5 && x_press_num == -2.5) {
			y_press_num = 2;
			x_press_num = 2.5;
		}
		if (y_press_num == -3) {
			y_press_num = 2;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		x_press_num -= 0.5;
		if (y_press_num == -2.5 && x_press_num == -2.5) {
			y_press_num = 2;
			x_press_num = 2.5;
		}
		if (x_press_num == -3) {
			x_press_num = 2.5;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		x_press_num += 0.5;
		if (y_press_num == 2 && x_press_num == 2.5) {
			y_press_num = -2.5;
			x_press_num = -2.5;
		}
		if (x_press_num == 3) {
			x_press_num = -2.5;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
		body_color_index++;
		body_color = colorPositions[body_color_index];
		if (body_color_index == 7) {
			body_color_index = 0;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		aspect += 0.2;
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		aspect -= 0.2;
	}
}
// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

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

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	fov -= (float)yoffset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 45.0f)
		fov = 45.0f;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void initializedGL(void) {
	// run only once
	// TODO:
	sendDataToOpenGL();
	installShaders();
	glEnable(GL_DEPTH_TEST);
}

int main(int argc, char* argv[]) {
	GLFWwindow* window;

	/* Initialize the glfw */
	if (!glfwInit()) {
		std::cout << "Failed to initialize GLFW" << std::endl;
		return -1;
	}
	/* glfw: configure; necessary for MAC */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	/* do not allow resizing */
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Assignment 1", NULL, NULL);
	if (!window) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback); // TODO
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	// TODO: mouse callback, etc.

	/* Initialize the glew */
	if (GLEW_OK != glewInit()) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}
	get_OpenGL_info();
	initializedGL();

	double jump_start_time = -1.0;
	double jump_end_time = -1.0;
	double jump_duration = 1.0; // 跳跃动作持续时间
	

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window)) {
		

		/* Render here */
		paintGL();


		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && jump_start_time < 0.0) {
			// 如果还没有跳跃，则记录跳跃开始时间
			jump_start_time = glfwGetTime();
			jump_end_time = jump_start_time + jump_duration;
		}

		// 计算模型位置
		double current_time = glfwGetTime();
		if (current_time >= jump_start_time && current_time <= jump_end_time) {
			// 如果在跳跃时间范围内，则计算跳跃位置
			float jump_progress = (current_time - jump_start_time) / jump_duration;
			float jump_height = 1.0f; // 跳跃高度
			z_press_num = jump_height * sin(jump_progress * PI);
		}
		else {
			// 如果不在最后一次跳跃的时间范围内，则重置跳跃时间记录
			jump_start_time = -1.0;
			jump_end_time = -1.0;
		}
		if (fish_x !=2.6f) {
			fish_x += 0.001f;
			if (fish_x > 2.5f) {
				fish_x = -2.5f;
			}
		}

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}
