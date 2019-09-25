#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <cmath>

#include "shader.hpp"
#include "texture.hpp"
#include "engine.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
GLFWwindow * AllInit();

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

double lastX = 0, lastY = 0;

const GLchar * vertexShaderSource = R"KEK(

#version 450 core
layout ( location = 0 ) in vec3 position;
layout ( location = 1 ) in vec2 textCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 tCoord;

void main() {
	gl_Position = projection * view * model * vec4( position, 1.0f );
	tCoord = vec2( textCoord.x, 1.0f - textCoord.y );
}

)KEK";


const GLchar * fragmentShaderSource = R"KEK(

#version 450 core

uniform sampler2D myText;
in vec2 tCoord;

out vec4 FragColor;

void main() {
	FragColor = texture( myText, tCoord );
}

)KEK";

glm::vec3 cameraPos( 0.0f, 0.0f, 50.0f );
glm::vec3 cameraFront( 0.0f, 0.0f, -1.0f );
glm::vec3 cameraUp( 0.0f, 1.0f, 0.0f );
GLfloat cameraSpeed = 0.05f;

snake snk( 10 );

double pitch = 0.0, yaw = -90.0;
bool firstMouse = true;

void mouse_callback( GLFWwindow * window, double xpos, double ypos );

int main()
{
	GLFWwindow * window  = AllInit();

	shader prog( vertexShaderSource, fragmentShaderSource );

	texture text( "stone.jpg" );

	glfwSwapInterval( 1 );
	glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
	glfwSetCursorPosCallback( window, mouse_callback );


    GLfloat vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

	std::vector< glm::vec3 > cubes = {
		glm::vec3( -1.0f, -1.0f, 0.0f ),
		glm::vec3( 1.0f, 1.0f, 0.0f ),
		glm::vec3( -1.0f, 1.0f, 0.0f ),
		glm::vec3( 1.0f, -1.0f, 0.0f )
	};

	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers( 1, &EBO );
	glBindVertexArray( VAO );

	glBindBuffer( GL_ARRAY_BUFFER, VBO );
	glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	glEnable( GL_DEPTH_TEST );

	int tick = 0;
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		glBindVertexArray( VAO );
		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		prog.Use();
		text.Bind();
		if ( ++tick % 30 == 0 )
			snk.update();

		for ( int i = 0; i < snk.size; ++i ) {

			glm::mat4 model( 1.0f ), view( 1.0f ), projection( 1.0f );

			model = glm::translate( model, snk.posits[ i ] );
			view = glm::lookAt( cameraPos, cameraPos + cameraFront, cameraUp );
			projection = glm::perspective( 45.0f, ( float ) SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f );

			prog.Matrix4( "model", glm::value_ptr( model ) );
			prog.Matrix4( "view", glm::value_ptr( view ) );
			prog.Matrix4( "projection", glm::value_ptr( projection ) );

			glDrawArrays( GL_TRIANGLES, 0, 36 );
		}
		glfwSwapBuffers( window );
		glfwPollEvents();
		std::cout << pitch << " " << yaw << std::endl;
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if ( glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS )
		cameraPos += cameraSpeed * cameraFront;
	if ( glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS )
		cameraPos -= cameraSpeed * cameraFront;
	if ( glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS )
		cameraPos -= cameraSpeed * glm::normalize( glm::cross( cameraFront, cameraUp ) );
	if ( glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS )
		cameraPos += cameraSpeed * glm::normalize( glm::cross( cameraFront, cameraUp ) );
	if ( glfwGetKey( window, GLFW_KEY_Q ) == GLFW_PRESS )
		cameraPos -= cameraSpeed * cameraUp;
	if ( glfwGetKey( window, GLFW_KEY_E ) == GLFW_PRESS )
		cameraPos += cameraSpeed * cameraUp;
	if ( glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS )
		snk.direction = 1;
	if ( glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS )
		snk.direction = 2;
	if ( glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS )
		snk.direction = 3;
	if ( glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS )
		snk.direction = 0;
}

void mouse_callback( GLFWwindow * window, double xpos, double ypos ) {
	if ( firstMouse ) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	GLfloat sens = 0.05f;
	xoffset *= sens;
	yoffset *= sens;

	yaw += xoffset;
	pitch += yoffset;

	if ( pitch > 89.0f )
		pitch = 89.0f;
	if ( pitch < -89.0f )
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos( glm::radians( pitch ) ) * cos( glm::radians( yaw ) );
	front.y = sin( glm::radians( pitch ) );
	front.z = cos( glm::radians( pitch ) ) * sin( glm::radians( yaw ) );
	cameraFront = glm::normalize( front ) ;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	SCR_HEIGHT = height;
	SCR_WIDTH = width;
	glViewport(0, 0, width, height);
}

GLFWwindow * AllInit() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if ( window == nullptr )
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return nullptr;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if ( !gladLoadGLLoader( ( GLADloadproc ) glfwGetProcAddress ) )
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return nullptr;
	}
	return window;
}
