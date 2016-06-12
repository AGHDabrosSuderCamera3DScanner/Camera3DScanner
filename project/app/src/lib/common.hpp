#ifndef COMMON_HPP
#define COMMON_HPP

#include "defines.hpp"
//#include <string>

namespace COMMON
{
	void raise_error( const char * str );
	//void raise_error( const std::string& str );
	
	void raise_perror( const char * str );
	//void raise_perror( const std::string& str );
}

#endif // COMMON_HPP
