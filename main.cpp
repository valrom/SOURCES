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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
GLFWwindow * AllInit();

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

double lastX = 0, lastY = 0;

const GLchar * vertexShaderSource = R"KEK(

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3( model * vec4(aPos, 1.0));
    Normal = vec3( model * vec4( aNormal, 0.0 ) );  
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}

)KEK";


const GLchar * fragmentShaderSource = R"KEK(

#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
  
uniform vec3 lightPos; 
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 viewPos;

void main()
{
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  
        
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
} 

)KEK";

const GLchar * lightShader = R"KEK(

#version 330 core
out vec4 FragColor;
void main() { FragColor = vec4( 1.0f ); }
)KEK";

glm::vec3 cameraPos( 0.0f, 0, -3.0f );
glm::vec3 cameraFront( 0.0f, 0.0f, -1.0f );
glm::vec3 cameraUp( 0.0f, 1.0f, 0.0f );
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
GLfloat cameraSpeed = 0.05f;


double pitch = 0.0, yaw = -90.0;
bool firstMouse = true;

void mouse_callback( GLFWwindow * window, double xpos, double ypos );

int main()
{
	GLFWwindow * window  = AllInit();

	shader prog( vertexShaderSource, fragmentShaderSource );
	shader light( vertexShaderSource, lightShader );

	texture text( "stone.jpg" );

	glfwSwapInterval( 1 );
	glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
	glfwSetCursorPosCallback( window, mouse_callback );


	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers( 1, &EBO );
	glBindVertexArray( VAO );

	glBindBuffer( GL_ARRAY_BUFFER, VBO );
	glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	glEnable( GL_DEPTH_TEST );

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		glBindVertexArray( VAO );
		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		prog.Use();
		text.Bind();

		glm::mat4 model( 1.0f ),
			view = glm::lookAt( cameraPos, cameraPos + cameraFront, cameraUp ),
			projection = glm::perspective( 45.0f, ( float ) SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f );

		// lightPos.x = sin( glfwGetTime() );
		// lightPos.z = cos( glfwGetTime() );

		// lightPos = cameraPos;

		prog.Matrix4( "model", glm::value_ptr( model ) );
		prog.Matrix4( "view", glm::value_ptr( view ) );
		prog.Matrix4( "projection", glm::value_ptr( projection ) );
		prog.Uniform( "objectColor", 1.0f, 0.5f, 0.31f );
		prog.Uniform( "lightColor", 1.0f, 1.0f, 1.0f );
		prog.Uniform( "lightPos", lightPos.x, lightPos.y, lightPos.y );
		prog.Uniform( "viewPos", cameraPos.x, cameraPos.y, cameraPos.z );

		glDrawArrays( GL_TRIANGLES, 0, 36 );

		glm::mat4 liMod( 1.0f );
		liMod = glm::translate( liMod, lightPos );
		liMod = glm::scale( liMod, glm::vec3( 0.2f ) );

		light.Use();

		light.Matrix4( "model", glm::value_ptr( liMod ) );
		light.Matrix4( "view", glm::value_ptr( view ) );
		light.Matrix4( "projection", glm::value_ptr( projection ) );
		light.Uniform( "objColor", 1.0f, 0.5f, 0.31f );
		light.Uniform( "lightColor", 1.0f, 1.0f, 1.0f );
		light.Uniform( "lightPos", lightPos.x, lightPos.y, lightPos.y );

		glDrawArrays( GL_TRIANGLES, 0, 36 );

		glfwSwapBuffers( window );
		glfwPollEvents();
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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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
