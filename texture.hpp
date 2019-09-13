#pragma once

#include <glad/glad.h>
#include <stb_image.h>
#include <iostream>

class texture {
public:
	GLuint tex;
public:
	texture( const char * text );
	void Bind();

	~texture();
};
