#ifndef DEFINES_H
#define DEFINES_H

#define DEBUGGING true
#ifdef DEBUGGING

#include <cstdio>
#define DEBUG(x) do { if( DEBUGGING ) fprintf( stderr, "%s\n", x );  } while( 0 );
#define DEBUG_INT(x) do { if( DEBUGGING ) fprintf( stderr, "%u\n", (unsigned int)x );  } while( 0 );

#endif // DEBUGGING

#endif // DEFINES_H
