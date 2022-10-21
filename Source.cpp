#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include<SOIL2/SOIL2.h>
#include<iostream>
using namespace std;

int width, height;
const double PI = 3.14159;
const float toRadians = PI / 180.0f;
GLuint LegsVBO, planeVBO, LegsVAO, planeVAO, boxVAO, boxVBO, EBO;
GLuint lamp_VAO, lamp_VBO, lamp_EBO;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

glm::mat4 viewMatix;

GLfloat fov = 45.f;

glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);

glm::vec3 traget = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 cameraDirection = glm::normalize(cameraPosition - traget);

glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 cameraRight = glm::normalize(glm::cross(worldUp, cameraDirection));

glm::vec3 cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight));

glm::vec3 cameraFront = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f));

glm::vec3 lightPosition(-1.5f, 4.0f, -1.5f);

glm::vec3 getTraget();

void TransformCamera();

bool keys[1024], mouseButtons[3];

bool isPanning = false, isOrbiting = false;

GLfloat radius = 3.f, rawYaw = 0.f, rawPicth = 0.f, degYaw, degPicth;

GLfloat deltaTime = 0.f, lastFrame = 0.f;
GLfloat lastX = 320, lastY = 240, xChange, yChange;
bool firstMouseMove = true;

void Initcamera();

void WASD_EQ_ProcessInput(GLFWwindow* window);

void To3D();

// Draw Primitive(s)
void drawtable()
{
	/* Triangle drawing starts here!*/
	glBindVertexArray(LegsVAO); // Enable triangle VAO (references triangle VBO and VA associations)		
	glDrawArrays(GL_TRIANGLES, 0, 100); // Render primitive or execute shaders
	/* Triangle drawing ends here! */

	/* Plane drawing starts here!*/
	glBindVertexArray(planeVAO); // Enable plane VAO (references triangle VBO and VA associations)		
	glDrawArrays(GL_TRIANGLES, 0, 100); // Render primitive or execute shaders
	/* Plane drawing end here!*/
}

void draw_box() {
	/* BOX drawing starts here!*/
	glBindVertexArray(boxVAO); // Enable plane VAO (references triangle VBO and VA associations)		
	glDrawArrays(GL_TRIANGLES, 0, 100); // Render primitive or execute shaders
	/* Plane drawing end here!*/
}

void draw_lamp()
{

	/* lamp drawing starts here!*/
	glBindVertexArray(lamp_VAO); // Enable plane VAO (references triangle VBO and VA associations)		
	glDrawArrays(GL_TRIANGLES, 0, 25); // Render primitive or execute shaders
	/* lamp drawing end here!*/
}

// Create and Compile Shaders
static GLuint CompileShader(const string& source, GLuint shaderType)
{
	// Create Shader object
	GLuint shaderID = glCreateShader(shaderType);
	const char* src = source.c_str();

	// Attach source code to Shader object
	glShaderSource(shaderID, 1, &src, nullptr);

	// Compile Shader
	glCompileShader(shaderID);

	// Return ID of Compiled shader
	return shaderID;

}

// Create Program Object
static GLuint CreateShaderProgram(const string& vertexShader, const string& fragmentShader)
{
	// Compile vertex shader
	GLuint vertexShaderComp = CompileShader(vertexShader, GL_VERTEX_SHADER);

	// Compile fragment shader
	GLuint fragmentShaderComp = CompileShader(fragmentShader, GL_FRAGMENT_SHADER);

	// Create program object
	GLuint shaderProgram = glCreateProgram();

	// Attach vertex and fragment shaders to program object
	glAttachShader(shaderProgram, vertexShaderComp);
	glAttachShader(shaderProgram, fragmentShaderComp);

	// Link shaders to create executable
	glLinkProgram(shaderProgram);

	// Delete compiled vertex and fragment shaders
	glDeleteShader(vertexShaderComp);
	glDeleteShader(fragmentShaderComp);

	// Return Shader Program
	return shaderProgram;

}


int main(void)
{
	width = 640; height = 480;

	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(width, height, "Main Window", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwSetKeyCallback(window, key_callback);

	glfwSetCursorPosCallback(window, cursor_position_callback);

	glfwSetMouseButtonCallback(window, mouse_button_callback);

	glfwSetScrollCallback(window, scroll_callback);



	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	if (glewInit() != GLEW_OK)
		cout << "Error!" << endl;

	GLfloat verticesLegs[] = {
		//front of the leg1
		0.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,//Red
		0.0,0.0,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,//Green
		0.0,1.0,
		1.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,

		0.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,//Red
		0.0,0.0,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,//Green
		0.0,1.0,

		//back of the leg1
		0.0f, 1.0f, -1.0f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,
		1.0f, -1.0f, -1.0f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0,
		1.0f, 1.0f, -1.0f,
		1.0f, 0.0f, 0.0f,//Red
		0.0,0.0,

		0.0f, 1.0f, -1.0f,
		0.0f, 1.0f, 0.0f,//Green
		0.0,1.0,
		0.0f, -1.0f, -1.0f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,
		1.0f, -1.0f, -1.0f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0,

		//left of the leg1
		0.0f, 1.0f,-1.0f,
		1.0f, 0.0f, 0.0f,//Red
		0.0,0.0,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,//Green
		0.0,1.0,
		0.0f, -1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,

		0.0f, 1.0f,-1.0f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0,
		0.0f, -1.0f, -1.0f,
		1.0f, 0.0f, 0.0f,//Red
		0.0,0.0,
		0.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,//Green
		0.0,1.0,

		// right of the leg1
		1.0f, 1.0f,-1.0f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,
		1.0f, -1.0f, -1.0f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0,
		1.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,//Red
		0.0,0.0,

		1.0f, -1.0f,-1.0f,
		0.0f, 1.0f, 0.0f,//Green
		0.0,1.0,
		1.0f, -1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,
		1.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0,

		//front of the leg2
		0.0f, 1.0f, -3.0f,
		1.0f, 0.0f, 0.0f,//Red
		0.0,0.0,
		1.0f, -1.0f, -3.0f,
		0.0f, 1.0f, 0.0f,//Green
		0.0,1.0,
		1.0f, 1.0f, -3.0f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,

		0.0f, 1.0f, -3.0f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0,
		0.0f, -1.0f, -3.0f,
		1.0f, 0.0f, 0.0f,//Red
		0.0,0.0,
		1.0f, -1.0f, -3.0f,
		0.0f, 1.0f, 0.0f,//Green
		0.0,1.0,

		//back of the leg2
		0.0f, 1.0f, -4.0f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,
		1.0f, -1.0f, -4.0f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0,
		1.0f, 1.0f, -4.0f,
		1.0f, 0.0f, 0.0f,//Red
		0.0,0.0,

		0.0f, 1.0f, -4.0f,
		0.0f, 1.0f, 0.0f,//Green
		0.0,1.0,
		0.0f, -1.0f, -4.0f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,
		1.0f, -1.0f, -4.0f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0,

		//left of the leg2
		0.0f, 1.0f,-4.0f,
		1.0f, 0.0f, 0.0f,//Red
		0.0,0.0,
		0.0f, 1.0f, -3.0f,
		0.0f, 1.0f, 0.0f,//Green
		0.0,1.0,
		0.0f, -1.0f, -3.0f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,

		0.0f, 1.0f,-4.0f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0,
		0.0f, -1.0f, -4.0f,
		1.0f, 0.0f, 0.0f,//Red
		0.0,0.0,
		0.0f, -1.0f, -3.0f,
		0.0f, 1.0f, 0.0f,//Green
		0.0,1.0,

		// right of the leg2
		1.0f, 1.0f,-4.0f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,
		1.0f, -1.0f, -4.0f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0,
		1.0f, 1.0f, -3.0f,
		1.0f, 0.0f, 0.0f,//Red
		0.0,0.0,

		1.0f, -1.0f,-4.0f,
		0.0f, 1.0f, 0.0f,//Green
		0.0,1.0,
		1.0f, -1.0f, -3.0f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,
		1.0f, 1.0f, -3.0f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0,


		//front of the leg3
		-4.0f, 1.0f, -3.0f,
		1.0f, 0.0f, 0.0f,//Red
		0.0,0.0,
		-3.0f, -1.0f, -3.0f,
		0.0f, 1.0f, 0.0f,//Green
		0.0,1.0,
		-3.0f, 1.0f, -3.0f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,

		-4.0f, 1.0f, -3.0f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0,
		-4.0f, -1.0f, -3.0f,
		1.0f, 0.0f, 0.0f,//Red
		0.0,0.0,
		-3.0f, -1.0f, -3.0f,
		0.0f, 1.0f, 0.0f,//Green
		0.0,1.0,

		//back of the leg3
		-4.0f, 1.0f, -4.0f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,
		-3.0f, -1.0f, -4.0f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0,
		-3.0f, 1.0f, -4.0f,
		1.0f, 0.0f, 0.0f,//Red
		0.0,0.0,

		-4.0f, 1.0f, -4.0f,
		0.0f, 1.0f, 0.0f,//Green
		0.0,1.0,
		-4.0f, -1.0f, -4.0f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,
		-3.0f, -1.0f, -4.0f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0,

		//left of the leg3
		-4.0f, 1.0f,-4.0f,
		1.0f, 0.0f, 0.0f,//Red
		0.0,0.0,
		-4.0f, 1.0f, -3.0f,
		0.0f, 1.0f, 0.0f,//Green
		0.0,1.0,
		-4.0f, -1.0f, -3.0f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,

		-4.0f, 1.0f,-4.0f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0,
		-4.0f, -1.0f, -4.0f,
		1.0f, 0.0f, 0.0f,//Red
		0.0,0.0,
		-4.0f, -1.0f, -3.0f,
		0.0f, 1.0f, 0.0f,//Green
		0.0,1.0,

		// right of the leg3
		-3.0f, 1.0f,-4.0f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,
		-3.0f, -1.0f, -4.0f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0,
		-3.0f, 1.0f, -3.0f,
		1.0f, 0.0f, 0.0f,//Red
		0.0,0.0,

		-3.0f, -1.0f,-4.0f,
		0.0f, 1.0f, 0.0f,//Green
		0.0,1.0,
		-3.0f, -1.0f, -3.0f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,
		-3.0f, 1.0f, -3.0f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0,

		//front of the leg4
		-4.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,//Red
		0.0,0.0,
		-3.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,//Green
		0.0,1.0,
		-3.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,

		-4.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0,
		-4.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,//Red
		0.0,0.0,
		-3.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,//Green
		0.0,1.0,

		//back of the leg4
		-4.0f, 1.0f, -1.0f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,
		-3.0f, -1.0f, -1.0f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0,
		-3.0f, 1.0f, -1.0f,
		1.0f, 0.0f, 0.0f,//Red
		0.0,0.0,

		-4.0f, 1.0f, -1.0f,
		0.0f, 1.0f, 0.0f,//Green
		0.0,1.0,
		-4.0f, -1.0f, -1.0f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,
		-3.0f, -1.0f, -1.0f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0,

		//left of the leg4
		-4.0f, 1.0f,-1.0f,
		1.0f, 0.0f, 0.0f,//Red
		0.0,0.0,
		-4.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,//Green
		0.0,1.0,
		-4.0f, -1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,

		-4.0f, 1.0f,-1.0f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0,
		-4.0f, -1.0f, -1.0f,
		1.0f, 0.0f, 0.0f,//Red
		0.0,0.0,
		-4.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,//Green
		0.0,1.0,

		// right of the leg4
		-3.0f, 1.0f,-1.0f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,
		-3.0f, -1.0f, -1.0f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0,
		-3.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,//Red
		0.0,0.0,

		-3.0f, -1.0f,-1.0f,
		0.0f, 1.0f, 0.0f,//Green
		0.0,1.0,
		-3.0f, -1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,
		-3.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0,
	};

	GLfloat verticesPlane[] = {
		//top of the leg and bottom Plane of the table
		1.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,//Red
		0.0,0.0,
		1.0f, 1.0f, -4.0f,
		0.0f, 1.0f, 0.0f,//Green
		0.0,1.0,
		-4.0f, 1.0f, -4.0f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,

		-4.0f, 1.0f, -4.0f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0,
		-4.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,//Red
		0.0,0.0,
		1.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,//Green
		0.0,1.0,

		//top Plane of the table
		1.0f, 2.0f, 0.0f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,
		1.0f, 2.0f, -4.0f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0,
		-4.0f, 2.0f, -4.0f,
		1.0f, 0.0f, 0.0f,//Red
		0.0,0.0,

		-4.0f, 2.0f, -4.0f,
		0.0f, 1.0f, 0.0f,//Green
		0.0,1.0,
		-4.0f, 2.0f, 0.0f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,
		1.0f, 2.0f, 0.0f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0,

		//Front thickness 
		-4.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,//Red
		0.0,0.0,
		-4.0f, 2.0f, 0.0f,
		0.0f, 1.0f, 0.0f,//Green
		0.0,1.0,
		1.0f, 2.0f, 0.0f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,

		-4.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0,
		1.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,//Red
		0.0,0.0,
		1.0f, 2.0f, 0.0f,
		0.0f, 1.0f, 0.0f,//Green
		0.0,1.0,

		//Back thickness 
		-4.0f, 1.0f, -4.0f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,
		-4.0f, 2.0f, -4.0f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0,
		1.0f, 2.0f, -4.0f,
		1.0f, 0.0f, 0.0f,//Red
		0.0,0.0,

		-4.0f, 1.0f, -4.0f,
		0.0f, 1.0f, 0.0f,//Green
		0.0,1.0,
		1.0f, 1.0f, -4.0f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,
		1.0f, 2.0f, -4.0f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0,

		//Right thickness 
		1.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,//Red
		0.0,0.0,
		1.0f, 2.0f, -4.0f,
		0.0f, 1.0f, 0.0f,//Green
		0.0,1.0,
		1.0f, 1.0f, -4.0f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,

		1.0f, 2.0f, 0.0f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0,
		1.0f, 2.0f, -4.0f,
		1.0f, 0.0f, 0.0f,//Red
		0.0,0.0,
		1.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,//Green
		0.0,1.0,

		//Left thickness 
		-4.0f, 2.0f, -4.0f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,
		-4.0f, 1.0f, -4.0f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0,
		-4.0f, 2.0f, 0.0f,
		1.0f, 0.0f, 0.0f,//Red
		0.0,0.0,

		-4.0f, 2.0f, 0.0f,
		0.0f, 1.0f, 0.0f,//Green
		0.0,1.0,
		-4.0f, 1.0f, -4.0f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,
		-4.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0
	};

	GLfloat verticesBox[] = {
		//front of the box
		-2.0f, 3.0f, -1.5f,
		1.0f, 0.0f, 0.0f,//Red
		0.0,0.0,
		-2.0f, 2.0f, -1.5f,
		0.0f, 1.0f, 0.0f,//Green
		0.0,1.0,
		-1.5f, 3.0f,-1.5f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,
		-1.5f, 3.0f, -1.5f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0,
		-2.0f, 2.0f, -1.5f,
		1.0f, 0.0f, 0.0f,//Red
		0.0,0.0,
		-1.5f, 2.0f, -1.5f,
		0.0f, 1.0f, 0.0f,//Green
		0.0,1.0,

		//back of the box
		-2.0f, 3.0, -2.5f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,
		-2.0f, 2.0f, -2.5f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0,
		-1.5f, 3.0f, -2.5f,
		1.0f, 0.0f, 0.0f,//Red
		0.0,0.0,
		-1.5f, 3.0f, -2.5f,
		0.0f, 1.0f, 0.0f,//Green
		0.0,1.0,
		-2.0f, 2.0f, -2.5f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,
		-1.5f, 2.0f, -2.5f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0,

		//left of the box
		-2.0f, 3.0f,-2.5f,
		1.0f, 0.0f, 0.0f,//Red
		0.0,0.0,
		-2.0f, 3.0f, -1.5f,
		0.0f, 1.0f, 0.0f,//Green
		0.0,1.0,
		-2.0f, 2.0f, -2.5f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,
		-2.0f, 2.0f, -2.5f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0,
		-2.0f, 3.0f, -1.5f,
		1.0f, 0.0f, 0.0f,//Red
		0.0,0.0,
		-2.0f, 2.0f, -1.5f,
		0.0f, 1.0f, 0.0f,//Green
		0.0,1.0,

		// right of the box
		-1.5f, 3.0f,-2.5f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,
		-1.5f, 3.0f,-1.5f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0,
		-1.5f, 2.0f, -2.5f,
		1.0f, 0.0f, 0.0f,//Red
		0.0,0.0,
		-1.5f, 2.0f, -2.5f,
		0.0f, 1.0f, 0.0f,//Green
		0.0,1.0,
		-1.5f, 3.0f,-1.5f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,
		-1.5f, 2.0f,-1.5f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0,

		// top of the box
		-2.0, 3.0f,-2.5f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,
		-1.5f, 3.0f,-2.5f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0,
		-2.0f, 3.0f, -1.5f,
		1.0f, 0.0f, 0.0f,//Red
		0.0,0.0,
		-1.5f, 3.0f,-2.5f,
		0.0f, 1.0f, 0.0f,//Green
		0.0,1.0,
		-2.0f, 3.0f, -1.5f,
		0.0f, 0.0f, 1.0f,//Blue
		1.0,0.0,
		-1.5f, 3.0f,-1.5f,
		1.0f, 0.0f, 1.0f,//Pup
		1.0,1.0,
	};

	GLfloat lamp_vertices[] = {
		//front of the lamp
		0.0f, 0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,
		0.0f, 0.5f, 0.0f,
		0.0f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,

		//back of the lamp
		0.0f, 0.5, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, 0.5f, -0.5f,
		0.0f, 0.5f, -0.5f,
		0.0f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,

		//left of the lamp
		0.0f, 0.5f,-0.5f,
		0.0f, 0.5f, 0.0f,
		0.0f, -0.5f, 0.0f,
		0.0f, 0.5f,-0.5f,
		0.0f, -0.5f, -0.5f,
		0.0f, -0.5f, 0.0f,

		// right of the lamp
		0.5f, 0.5f,-0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, 0.5f, 0.0f,
		0.5f, -0.5f,-0.5f,
		0.5f, -0.5f, 0.0f,
		0.5f, 0.5f, 0.0f
	};


	// Define element indices
	GLubyte indices[] = {
		0, 1, 2,
		1, 2, 3
	};

	glm::vec3 planePositions[] = {
		glm::vec3(0.0f,0.0f,0.5f),
		glm::vec3(0.5f,0.0f,0.0f),
		glm::vec3(0.0f,0.0f,-0.5f),
		glm::vec3(-0.5f,0.0f,0.0f)
	};

	glm::float32 planeRotations[] = {
		0.0f,90.0f,0.0f,90.0f
	};

	glEnable(GL_DEPTH_TEST);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glGenBuffers(1, &LegsVBO); // Create VBO
	glGenBuffers(1, &EBO); // Create EBO
	glGenVertexArrays(1, &LegsVAO); // Create VOA
	glBindVertexArray(LegsVAO);
	// VBO and EBO Placed in User-Defined VAO
	glBindBuffer(GL_ARRAY_BUFFER, LegsVBO); // Select VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // Select EBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesLegs), verticesLegs, GL_STATIC_DRAW); // Load vertex attributes
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // Load indices 			
																					// Specify attribute location and layout to GPU
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0); // Associate VBO (vertex positions xyz) with VA (Vertex Attribute)
	glEnableVertexAttribArray(0); // Enable VA 0

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat))); // Associate VBO (vertex colors rgb) with VA (Vertex Attribute)
	glEnableVertexAttribArray(1); // Enable VA

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat))); // Associate VBO (vertex colors rgb) with VA (Vertex Attribute)
	glEnableVertexAttribArray(2); // Enable VA
	glBindVertexArray(0); // Unbind VAO (Optional but recommended)


	glGenVertexArrays(1, &planeVAO); // Create VAO
	glGenBuffers(1, &planeVBO); // Create VBO	
	glBindVertexArray(planeVAO); // Activate VAO for VBO association
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO); // Enable VBO		
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesPlane), verticesPlane, GL_STATIC_DRAW); // Copy or load Vertex data to VBO

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0); // Associate VBO (vertex positions xyz) with VA (Vertex Attribute)
	glEnableVertexAttribArray(0); // Enable VA 0

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat))); // Associate VBO (vertex colors rgb) with VA (Vertex Attribute)
	glEnableVertexAttribArray(1); // Enable VA

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat))); // Associate VBO (vertex colors rgb) with VA (Vertex Attribute)
	glEnableVertexAttribArray(2); // Enable VA
	glBindVertexArray(0); // Unbind VAO (Optional but recommended)

	glGenVertexArrays(1, &boxVAO); // Create VAO
	glGenBuffers(1, &boxVBO); // Create VBO	
	glBindVertexArray(boxVAO); // Activate VAO for VBO association
	glBindBuffer(GL_ARRAY_BUFFER, boxVBO); // Enable VBO		
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesBox), verticesBox, GL_STATIC_DRAW); // Copy or load Vertex data to VBO

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0); // Associate VBO (vertex positions xyz) with VA (Vertex Attribute)
	glEnableVertexAttribArray(0); // Enable VA 0

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat))); // Associate VBO (vertex colors rgb) with VA (Vertex Attribute)
	glEnableVertexAttribArray(1); // Enable VA

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat))); // Associate VBO (vertex colors rgb) with VA (Vertex Attribute)
	glEnableVertexAttribArray(2); // Enable VA
	glBindVertexArray(0); // Unbind VAO (Optional but recommended)


	glGenBuffers(1, &lamp_VBO); // Create VBO
	glGenBuffers(1, &lamp_EBO); // Create EBO

	glGenVertexArrays(1, &lamp_VAO); // Create VOA
	glBindVertexArray(lamp_VAO);

	// VBO and EBO Placed in User-Defined VAO
	glBindBuffer(GL_ARRAY_BUFFER, lamp_VBO); // Select VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lamp_EBO); // Select EBO


	glBufferData(GL_ARRAY_BUFFER, sizeof(lamp_vertices), lamp_vertices, GL_STATIC_DRAW); // Load vertex attributes
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // Load indices 

																					 // Specify attribute location and layout to GPU
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (GLvoid*)0); // Associate VBO (vertex positions xyz) with VA (Vertex Attribute)
	glEnableVertexAttribArray(0); // Enable VA 0

	glBindVertexArray(0); // Unbind VAO (Optional but recommended) // Unbind VOA or close off (Must call VOA explicitly in loop)



	int	tabelTexWidth, tabelTexHeight;
	unsigned char* tabelImage = SOIL_load_image("tabel.png", &tabelTexWidth, &tabelTexHeight, 0, SOIL_LOAD_RGB);

	GLuint tabelTexture;
	glGenTextures(1, &tabelTexture);
	glBindTexture(GL_TEXTURE_2D, tabelTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tabelTexWidth, tabelTexHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, tabelImage);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(tabelImage);
	glBindTexture(GL_TEXTURE_2D, 0);


	int	boxTexWidth, boxTexHeight;
	unsigned char* boxImage = SOIL_load_image("box.png", &boxTexWidth, &boxTexHeight, 0, SOIL_LOAD_RGB);
	GLuint boxTexture;
	glGenTextures(1, &boxTexture);
	glBindTexture(GL_TEXTURE_2D, boxTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, boxTexWidth, boxTexHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, boxImage);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(boxImage);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Vertex shader source code
	string vertexShaderSource =
		"#version 330 core\n"
		"layout(location = 0) in vec3 vPosition;"
		"layout(location = 1) in vec3 aColor;"
		"layout(location = 2) in vec2 texCoord;"
		"out vec3 oColor;"
		"out vec2 oTexCoord;"
		"uniform mat4 model;"
		"uniform mat4 view;"
		"uniform mat4 projection;"
		"void main()\n"
		"{\n"
		"gl_Position = projection * view * model * vec4(vPosition.x,vPosition.y,vPosition.z,1.0);"
		"oColor = aColor;"
		"oTexCoord = texCoord;"
		"}\n";

	// Fragment shader source code
	string fragmentShaderSource =
		"#version 330 core\n"
		"in vec3 oColor;"
		"in vec2 oTexCoord;"
		"out vec4 fragColor;"
		"uniform sampler2D myTexture;"
		"uniform vec3 objectColor;"
		"uniform vec3 objectColor2;"
		"uniform vec3 lightColor;"
		"uniform vec3 lightPos;"
		"void main()\n"
		"{\n"
		"float ambientStrength = 1.5f;"
		"vec3 ambient = ambientStrength * lightColor;"
		"vec3 result = ambient * objectColor;"
		"fragColor = texture(myTexture, oTexCoord) * vec4(result,1.0f);"
		"}\n";

	// LAMP V shader source code
	string lampVShaderSource =
		"#version 330 core\n"
		"layout(location = 0) in vec3 vPosition;"
		"uniform mat4 model;"
		"uniform mat4 view;"
		"uniform mat4 projection;"
		"void main()\n"
		"{\n"
		"gl_Position = projection * view * model * vec4(vPosition.x,vPosition.y,vPosition.z,1.0);"
		"}\n";

	// LAMP Fragment shader source code
	string lampFShaderSource =
		"#version 330 core\n"
		"out vec4 fragColor;"
		"void main()\n"
		"{\n"
		"fragColor = vec4(vec3( 1.0f, 1.0f, 1.0f),1.0f);"
		"}\n";

	// Creating Shader Program
	GLuint shaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);

	GLuint lampShaderProgram = CreateShaderProgram(lampVShaderSource, lampFShaderSource);

	// Use Shader Program exe once
	//glUseProgram(shaderProgram);


	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		GLfloat CurrentFrame = glfwGetTime();
		deltaTime = CurrentFrame - lastFrame;
		lastFrame = CurrentFrame;

		WASD_EQ_ProcessInput(window);
		// Resize window and graphics simultaneously
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use Shader Program exe and select VAO before drawing 
		glUseProgram(shaderProgram); // Call Shader per-frame when updating attributes

		glm::mat4 projectionMatrix;

		viewMatix = glm::lookAt(cameraPosition, getTraget(), worldUp);

		projectionMatrix = glm::perspective(fov, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);



		GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
		GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
		GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");

		GLuint objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");
		GLuint objectColor2Loc = glGetUniformLocation(shaderProgram, "objectColor2");
		GLuint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
		GLuint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");

		glUniform3f(objectColorLoc, 0.76f, 0.71f, 0.69f);
		glUniform3f(objectColor2Loc, 0.0f, 0.1f, 0.5f);
		glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightPosLoc, lightPosition.x, lightPosition.y, lightPosition.z);

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatix));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

		glBindTexture(GL_TEXTURE_2D, tabelTexture);
		glBindVertexArray(LegsVAO); // User-defined VAO must be called before draw. 
		glm::mat4 modelMatix;
		modelMatix = glm::translate(modelMatix, glm::vec3(0.0f, -1.0f, -2.0f));
		modelMatix = glm::rotate(modelMatix, 0.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatix = glm::scale(modelMatix, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatix));

		glBindTexture(GL_TEXTURE_2D, tabelTexture);
		glBindVertexArray(planeVAO); // User-defined VAO must be called before draw.
		modelMatix = glm::translate(modelMatix, glm::vec3(0.0f, -1.0f, -2.0f));
		modelMatix = glm::rotate(modelMatix, 0.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatix = glm::scale(modelMatix, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatix));
		// Draw primitive(s)
		drawtable();

		glBindTexture(GL_TEXTURE_2D, boxTexture);
		glBindVertexArray(boxVAO); // User-defined VAO must be called before draw.
		modelMatix = glm::translate(modelMatix, glm::vec3(0.0f, 0.0f, 0.0f));
		modelMatix = glm::rotate(modelMatix, 0.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatix = glm::scale(modelMatix, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatix));
		// Draw primitive(s)
		draw_box();

		// Unbind Shader exe and VOA after drawing per frame
		glBindVertexArray(0); //Incase different VAO wii be used after
		glUseProgram(0); // Incase different shader will be used after

		glUseProgram(lampShaderProgram);

		glBindVertexArray(lamp_VAO); //Incase different VAO wii be used after

		GLuint lampModelLoc = glGetUniformLocation(lampShaderProgram, "model");
		GLuint lampViewLoc = glGetUniformLocation(lampShaderProgram, "view");
		GLuint lampProjectionLoc = glGetUniformLocation(lampShaderProgram, "projection");

		glUniformMatrix4fv(lampViewLoc, 1, GL_FALSE, glm::value_ptr(viewMatix));
		glUniformMatrix4fv(lampProjectionLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));


		modelMatix = glm::translate(modelMatix, lightPosition);
		modelMatix = glm::rotate(modelMatix, 0.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatix = glm::scale(modelMatix, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(lampModelLoc, 1, GL_FALSE, glm::value_ptr(modelMatix));
		draw_lamp();
		glBindVertexArray(0); //Incase different VAO wii be used after
		glUseProgram(0); // Incase different shader will be used after

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();

		TransformCamera();
	}

	//Clear GPU resources
	glDeleteVertexArrays(1, &LegsVAO);
	glDeleteBuffers(1, &LegsVBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &planeVAO);
	glDeleteBuffers(1, &planeVBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &lamp_VAO);
	glDeleteBuffers(1, &lamp_VBO);
	glDeleteBuffers(1, &lamp_EBO);
	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	//cout << "ASCII: " << key << endl;

	if (action == GLFW_PRESS)
		keys[key] = true;

	else if (action == GLFW_RELEASE)
		keys[key] = false;

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {

	/*f (yoffset > 0)
		cout << "scroll up: ";
	else if (yoffset < 0)
		cout << "scroll down: ";

	cout << yoffset << endl;*/

	if (fov >= 1.f && fov <= 45.f) {
		fov -= yoffset * 0.01f;

	}

	if (fov < 1.f)
		fov = 1.f;
	if (fov > 45.f)
		fov = 45.f;




}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {

	/*cout << "Mouse X; " << xpos << endl;
	cout << "Mouse Y; " << ypos << endl;*/

	if (firstMouseMove) {
		lastX = xpos;
		lastY = ypos;
		firstMouseMove = false;
	}

	xChange = xpos - lastX;
	yChange = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	if (isPanning) {
		GLfloat cameraSpeed = xChange * deltaTime;
		cameraPosition += cameraSpeed * cameraRight;

		cameraSpeed = yChange * deltaTime;
		cameraPosition += cameraSpeed * cameraUp;
	}

	if (isOrbiting) {
		rawYaw += xChange;
		rawPicth += yChange;
		degYaw = glm::radians(rawYaw);
		//degPicth = glm::radians(rawPicth);
		degPicth = glm::clamp(glm::radians(rawPicth), -glm::pi<float>() / 2.f + 1.f, glm::pi<float>() / 2.f - 1.f);
		cameraPosition.x = traget.x + radius * cosf(degPicth) * sinf(degYaw);
		cameraPosition.y = traget.y + radius * sinf(degPicth);
		cameraPosition.z = traget.z + radius * cosf(degPicth) * cosf(degYaw);
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {


	/*if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		cout << "MOUSE_BUTTON_LEFT clicked" << endl;

	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
		cout << "MOUSE_BUTTON_MIDDLE clicked" << endl;

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		cout << "MOUSE_BUTTON_RIGHT clicked" << endl;*/

	if (action == GLFW_PRESS)
		mouseButtons[button] = true;
	else if (action == GLFW_RELEASE)
		mouseButtons[button] = false;
}


glm::vec3 getTraget() {

	if (isPanning)
		traget = cameraPosition + cameraFront;

	return traget;

}

void TransformCamera() {

	if (keys[GLFW_KEY_LEFT_ALT] && mouseButtons[GLFW_MOUSE_BUTTON_MIDDLE])
		isPanning = true;
	else
		isPanning = false;

	if (keys[GLFW_KEY_LEFT_ALT] && mouseButtons[GLFW_MOUSE_BUTTON_LEFT])
		isOrbiting = true;
	else
		isOrbiting = false;

	if (keys[GLFW_KEY_P])
		if (cameraPosition != glm::vec3(0.0f, 0.0f, 3.0f))
			Initcamera();
		else
			To3D();
}

void Initcamera() {
	cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);

	traget = glm::vec3(0.0f, 0.0f, 0.0f);

	cameraDirection = glm::normalize(cameraPosition - traget);

	worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

	cameraRight = glm::normalize(glm::cross(worldUp, cameraDirection));

	cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight));

	cameraFront = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f));
}

void WASD_EQ_ProcessInput(GLFWwindow* window)
{
	static const float cameraSpeed = 2.5f;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraOffset = cameraSpeed * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPosition += cameraOffset * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPosition -= cameraOffset * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraOffset;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraOffset;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		cameraPosition -= cameraOffset * cameraUp;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		cameraPosition += cameraOffset * cameraUp;
}

void To3D() {

	cameraPosition = glm::vec3(-5.0f, 5.0f, 2.0f);

	traget = glm::vec3(-3.0f, 2.0f, -6.0f);

	cameraDirection = glm::normalize(cameraPosition - traget);

	worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

	cameraRight = glm::normalize(glm::cross(worldUp, cameraDirection));

	cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight));

	cameraFront = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f));

}