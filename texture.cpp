#include "texture.hpp"


texture::texture( const char * text ) : tex( 0 ) {

	// create texture
	int width, height, nrChan;
	unsigned char * image = stbi_load( text, &width, &height, &nrChan, 0 );

	if ( image == nullptr )
		std::cout << "Image not loaded: " << text << std::endl;

	glGenTextures( 1, &tex );

	glBindTexture( GL_TEXTURE_2D, tex );

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image );
	glGenerateMipmap( GL_TEXTURE_2D );

	stbi_image_free( image );

	glBindTexture( GL_TEXTURE_2D, 0 );
}

void texture::Bind() { glBindTexture( GL_TEXTURE_2D, tex ); }

texture::~texture() { glDeleteTextures( 1, &tex ); }
