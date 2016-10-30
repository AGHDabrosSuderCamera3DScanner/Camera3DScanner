#ifndef DEFINES_HPP
#define DEFINES_HPP

#define DEBUGGING true
#ifdef DEBUGGING

#include <cstdio>

#define DEBUG(x) do { if( DEBUGGING ) fprintf( stderr, "%s\n", (x) );  } while( 0 );
#define DEBUG_INT(x) do { if( DEBUGGING ) fprintf( stderr, "%u\n", (unsigned int)(x) );  } while( 0 );

#endif // DEBUGGING

#include <inttypes.h>

#define UINT        unsigned int
#define ULONG       unsigned long
#define ULONGLONG   unsigned long long
#define BYTE        unsigned char

#define UINT8	uint8_t
#define UINT16	uint16_t
#define UINT32	uint32_t
#define UINT64	uint64_t

#define SIZEOF(T) ( (size_t)( (T*)0 + 1 - 0 ) )

#define BITMAP16_INNER_TYPE   UINT16
#define CUBE16_INNER_TYPE     UINT16

#define BITMAP16    BITMAP16_INNER_TYPE[ SIZE * SIZE ]
#define CUBE16      CUBE16_INNER_TYPE[ SIZE * SIZE * SIZE ]

#define BITMAP16_SIZE   ( SIZE * SIZE * SIZEOF( BITMAP16_INNER_TYPE ) )
#define CUBE16_SIZE     ( SIZE * SIZE * SIZE * SIZEOF( CUBE16_INNER_TYPE ) )

#define RGB16(r,g,b)	( (UINT16)( ( ( ( (r) * 31 ) / 255 ) << 11 ) + \
						 			( ( ( (g) * 63 ) / 255 ) << 5 ) + \
									( ( ( (b) * 31 ) / 255 ) ) ) )

#define RED16(x)		( ( ( ( (x) & ( 31 << 11 ) ) >> 11 ) * 255 ) / 31 )
#define GREEN16(x)		( ( ( ( (x) & ( 63 <<  5 ) ) >>  5 ) * 255 ) / 63 )
#define BLUE16(x)		  ( ( ( (x) &   31 )                 * 255 ) / 31 )

#define VALUE16(x)		MAX3( RED16( x ), GREEN16( x ), BLUE16( x ) )


#define IF(c,t,f)	( c ? t : f )
#define MAX2(x,y)	( IF( (x) > (y), (x), (y) ) )
#define MAX3(x,y,z)	( MAX2( (x), MAX2( (y), (z) ) ) )

#define TABLE2D(T,x,y,sY)       T[ (x) * (sY) + (y) ]
#define TABLE3D(T,x,y,z,sY,sZ)  T[ (x) * (sY) * (sZ) + (y) * (sZ) + (z) ]

#define PI              3.14159265359
#define SIZE            128LL                // size of mesh ( SIZE x SIZE x SIZE )
#define L               ( SIZE * 5 )        // distance from camera to screen
#define CAMERA_ANGLE    ( 0.22268202868 )   // camera angle of vision
//#define MIN_COS         0.0000001
#define SIZE_2          ( SIZE / 2 )
#define D               ( L - SIZE )          // distance from camera to object cube
#define SHIFT(q)        ( (q) - ( SIZE_2 ) )

#define NUM_THREADS		4	// number of threads to use for processing

#endif // DEFINES_HPP

/******************************************************************************/
