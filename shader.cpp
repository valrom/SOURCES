#include "shader.hpp"
#include <iostream>

GLuint shader::Compile( const GLchar*& shadText, GLenum type ) {
	GLuint shad = glCreateShader( type );
	glShaderSource( shad, 1, &shadText, NULL );
	glCompileShader( shad );
	// Check for compile time errors
	GLint success;
	GLchar infoLog[ 512 ];
	glGetShaderiv( shad, GL_COMPILE_STATUS, &success );
	if ( !success )
	{
		glGetShaderInfoLog( shad, 512, NULL, infoLog );
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	return shad;
}

shader::shader( const GLchar * & vertText, const GLchar * & fragText ) {

	GLuint vert = Compile( vertText, GL_VERTEX_SHADER );
	GLuint frag = Compile( fragText, GL_FRAGMENT_SHADER );

	prog = glCreateProgram();
	glAttachShader( prog, vert );
	glAttachShader( prog, frag );
	glLinkProgram( prog );
	// Check for linking errors
	GLint success;
	GLchar infoLog[ 512 ];
	glGetProgramiv( prog, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog( prog, 512, NULL, infoLog );
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader( vert );
	glDeleteShader( frag );
}

void shader::Use() {
	glUseProgram( prog );
}

GLint shader::GetParam( const GLchar* text ) {
	return glGetUniformLocation( prog, text );
}

void shader::Uniform( const GLchar * text, float x, float y, float z, float t ) {
	GLint varLoc = glGetUniformLocation( prog, text );
	glUniform4f( varLoc, x, y, z, t );
}
void shader::Uniform( const GLchar * text, float x, float y, float z) {
	GLint varLoc = glGetUniformLocation( prog, text );
	glUniform3f( varLoc, x, y, z );
}
void shader::Uniform( const GLchar * text, float x, float y) {
	GLint varLoc = glGetUniformLocation( prog, text );
	glUniform2f( varLoc, x, y );
}
void shader::Uniform( const GLchar * text, float x ) {
	GLint varLoc = glGetUniformLocation( prog, text );
	glUniform1f( varLoc, x );
}

shader::~shader() {
	glDeleteProgram( prog );
}
