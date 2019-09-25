#include <glad/glad.h>
#include <vector>
#include <GLM/glm.hpp>

class snake {

public:
	std::vector< glm::vec3 > posits;
	unsigned int size;
	unsigned int direction;
	snake( int kek ) : posits( 100, glm::vec3( 0.0f, 0.0f, 0.0f ) ), size( kek ), direction( 0 ) {
		std::cout << "snake init" << std::endl;
	}
	void update() {
		for ( int i = size - 1; i > 0; i-- )
			posits[ i ] = posits[ i - 1 ];
		if ( direction == 0 )
			posits[ 0 ] += glm::vec3( 1.0f, 0.0f, 0.0f );
		else if ( direction == 1 )
			posits[ 0 ] += glm::vec3( 0.0f, 1.0f, 0.0f );
		else if ( direction == 2 )
			posits[ 0 ] += glm::vec3( -1.0f, 0.0f, 0.0f );
		else
			posits[ 0 ] += glm::vec3( 0.0f, -1.0f, 0.0f );

	}

};
