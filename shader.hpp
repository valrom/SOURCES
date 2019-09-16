#pragma once

#include <glad/glad.h>

class shader {
public:
	GLuint prog;
public:
	static GLuint Compile( const GLchar*& shadText, GLenum type );
	shader( const GLchar * & vertText, const GLchar * & fragText );
	void Use();
	GLint GetParam( const GLchar * text );
	void Uniform( const GLchar * text, float, float, float, float );
	void Uniform( const GLchar * text, float, float, float );
	void Uniform( const GLchar * text, float, float );
	void Uniform( const GLchar * text, float );

	void Matrix4( const GLchar * text, float *);

	~shader();
};
