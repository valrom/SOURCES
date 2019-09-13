#pragma once

#include <glad/glad.h>

class shader {
private:
	GLuint prog;
public:
	static GLuint Compile( const GLchar*& shadText, GLenum type );
	shader( const GLchar * & vertText, const GLchar * & fragText );
	void Use();
	GLint GetParam( const GLchar * text );
	void Uniform4f( const GLchar * text, float, float, float, float );

	~shader();
};
