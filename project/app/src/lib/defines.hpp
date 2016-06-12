#ifndef DEFINES_HPP
#define DEFINES_HPP

#define DEBUGGING true
#ifdef DEBUGGING

#include <cstdio>

#define DEBUG(x) do { if( DEBUGGING ) fprintf( stderr, "%s\n", x );  } while( 0 );
#define DEBUG_INT(x) do { if( DEBUGGING ) fprintf( stderr, "%u\n", (unsigned int)x );  } while( 0 );

#endif // DEBUGGING

#include <inttypes.h>

#define UINT8	uint8_t
#define UINT16	uint16_t
#define UINT32	uint32_t
#define UINT64	uint64_t

#define IF(c,t,f)	( c ? t : f )
#define MAX2(x,y)	( IF( x > y, x, y ) )
#define MAX3(x,y,z)	( MAX2( x, MAX2( y, z ) ) )

#define TABLE2D(T,x,y,sY)       T[x * sY + y]
#define TABLE3D(T,x,y,z,sY,sZ)  T[ x*sY*sZ + y*sZ + z ]

#endif // DEFINES_HPP

/******************************************************************************/
