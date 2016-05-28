#ifndef COMMON_H
#define COMMON_H

#include "defines.h"
#include <errno.h>
#include <stdlib.h>

void raise_error(char * str)
{
	fprintf(stderr, "Error: %s\n", str);
	exit(1);
}

void raise_perror(char * str)
{
	fprintf(stderr, "Error: %s\n", str);
	perror("Server");
	exit(1);
}

#endif // COMMON_H
