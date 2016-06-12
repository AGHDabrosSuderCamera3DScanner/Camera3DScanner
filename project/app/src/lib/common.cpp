#include "common.hpp"
#include <errno.h>
#include <stdlib.h>


/******************************************************************************/

void COMMON::raise_error( const char * str )
{
	fprintf(stderr, "Error: %s\n", str);
	exit(1);
}

/******************************************************************************/
/*
void COMMON::raise_error( const std::string& str )
{
	fprintf(stderr, "Error: %s\n", str);
	exit(1);
}
*/
/******************************************************************************/

void COMMON::raise_perror( const char * str )
{
	fprintf(stderr, "Error: %s\n", str);
	perror("Error");
	exit(1);
}

/******************************************************************************/
/*
void COMMON::raise_perror( const std::string& str )
{
	fprintf(stderr, "Error: %s\n", str);
	perror("Error");
	exit(1);
}
*/
/******************************************************************************/
