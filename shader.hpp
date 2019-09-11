#pragma once

#define GLEW_STATIC
#include <GL/glew.h>

class shader {
private:
	GLuint prog;
public:
	static GLuint Compile( const GLchar*& shadText, GLenum type );
	shader( const GLchar * & vertText, const GLchar * & fragText );
	void Use();
	GLint GetParam( const GLchar * text );
};