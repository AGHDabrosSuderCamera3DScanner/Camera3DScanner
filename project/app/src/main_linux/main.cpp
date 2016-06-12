#include "common.hpp"
#include "core.hpp"
#include <string>
#include <string.h>
#include <cmath>
#include <cstdlib>

#define SIZE          128
#define SIZE_2        SIZE/2
#define L             ( SIZE * 5 )
#define D             ( L - SIZE )
#define SHIFT(q)      ( (q) - ( SIZE_2 ) )
#define PI            3.14159265359
#define COUNT         32
#define ALPHA(q)      RAD( ( 360.0 * ( q ) ) / COUNT )
#define RAD(q)        ( PI * ( q ) / 180.0 )
#define SCREEN        ( ( ( L * L * SIZE ) / ( 2.0 * L - SIZE ) ) / static_cast<double>( sqrt( L * L - pow( ( L * SIZE ) / ( 2.0 * L - SIZE ) , 2 ) ) ) )


using namespace COMMON;

int count = 0;

/******************************************************************************/

bool isInCircle( unsigned int x, unsigned int y )
{
	x -= SIZE / 2;
	y -= SIZE / 2;
	x *= x;
	y *= y;

	if( x + y <= ( SIZE / 2 ) *( SIZE / 2 ) )
		return true;

	return false;
}

/******************************************************************************/

void generateCircle( UINT16 * table, size_t size )
{
	if( table == NULL || size <= 0 )
	{
		return;
	}

	unsigned int i, j;

	for( i = 0; i < size; i++ )
	{
		for( j = 0; j < size; j++ )
		{
			if( isInCircle( i, j ) )
				TABLE2D( table, i, j, size ) = RGB16( 100, 100, 100 );
			else
				TABLE2D( table, i, j, size ) = RGB16( 255, 255, 255 );
		}
	}

	return;
}

/******************************************************************************/

void printTable2D( UINT16 * table, size_t sizeX, size_t sizeY )
{
	if( table == NULL || sizeX <= 0 || sizeY <= 0 )
	{
		return;
	}

	unsigned int i, j;

	for( i = 0; i < sizeX; i++ )
	{
		for( j = 0; j < sizeY; j++ )
		{
			fprintf( stdout, "%4u", VALUE16( TABLE2D( table, i, j, sizeX ) ) );
		}

		fprintf( stdout, "\n" );
	}

	return;
}

/******************************************************************************/

void printTable3D( UINT16 * table, size_t sizeX, size_t sizeY, size_t sizeZ )
{
	if( table == NULL || sizeX <= 0 || sizeY <= 0 || sizeZ <= 0 )
	{
		return;
	}

	unsigned int i, j, k;

	for( i = 0; i < sizeX; i++ )
	{
		for( j = 0; j < sizeY; j++ )
		{
			for( k = 0; k < sizeZ; k++ )
			{
				if( TABLE3D( table, i, j, k, sizeY, sizeZ ) == count )
					//fprintf( stdout, "%d,%d,%d%c%c", i, j, k, 13, 10 );
					fprintf( stdout, "%d %d %d\n", i, j, k );
			}
		}
	}

	return;
}

/******************************************************************************/

int main( int argc, char * argv[] )
{
	UINT16 * table2D = new UINT16[SIZE * SIZE];
	UINT16 * table3D = new UINT16[SIZE * SIZE * SIZE];

	memset( table2D, 0, SIZE * SIZE * sizeof( UINT16 ) );
	memset( table3D, 0, SIZE * SIZE * SIZE * sizeof( UINT16 ) );


	generateCircle( table2D, SIZE );
	//printTable2D( table2D, SIZE, SIZE );
	//printf( "\n" );

	CORE::ImageAnalysis imageAnalysis;
	CORE::ProcessedBitmap16 * processedBitmap16 = NULL;

	imageAnalysis.provideBitmap16( table2D, SIZE, SIZE );

	if( imageAnalysis.isBitmap16Valid() )
	{
		processedBitmap16 = imageAnalysis.processBitmap16();

		if( processedBitmap16 == NULL )
		{
			raise_error( "Processed bitmap16 is null" );
		}
		else
		{
			if( processedBitmap16->isValid() )
			{
				//printTable2D( processedBitmap16->m_bitmap, processedBitmap16->m_sizeX, processedBitmap16->m_sizeY );
			}
			else
			{
				raise_error( "Processed bitmap16 is not valid" );
			}
		}
	}


	int x0, y0, x, y, z;
	int angle;

#define beta ( ALPHA( angle ) )

#define COS(q) fabs(cos( q ))
#define SIN(q) fabs(sin( q ))
#define TAN(q) fabs(tan( q ))

	for( angle = 0; angle < COUNT ; angle++ )
	{
	    if(COS(beta) > 0.0001)
	    {
	        count++;
            for( x = 0; x < SIZE; x++ )
            {
                for( y = 0; y < SIZE; y++ )
                {
                    for( z = 0; z < SIZE; z++ )
                    {

#define h ( abs( SHIFT( x ) ) )
#define g ( h * TAN( beta ) )
#define f ( h / COS( beta ) )
#define j ( fabs( SIZE_2 - z ) - g )
#define e ( j * SIN( beta ) )
#define c ( j * COS( beta ) )
#define b ( fabs( SIZE_2 - c ) )
#define a ( D )

                        //x0 = ( L * SHIFT( x ) ) / ( D + z ); // bez kąta
                        x0 = static_cast<int>( L * ( e + f ) / ( a + b ) );
                        x0 = static_cast<double>(x0) * SIZE  / SCREEN;
                        x0 += SIZE / 2;

                        //y0 = ( L * SHIFT( y ) ) / ( D + z ); // bez kąta
                        #define result ( abs( SIZE_2 - static_cast<double>(z) ) * COS( beta ) + abs( SHIFT( x ) ) * SIN( beta ) + SIZE_2 )
                        y0 = static_cast<int>( abs( SHIFT( y ) ) * static_cast<double>(L) / ( L - result ) ) ;
                        y0 = y0 * SIZE / SCREEN;
                        y0 += SIZE / 2;

                        if( x0 < SIZE && x0 >= 0 && y0 < SIZE && y0 >= 0 )
                        {
                            if( VALUE16( TABLE2D( processedBitmap16->m_bitmap, x0, y0, SIZE ) ) == 0 )
                            {

                                TABLE3D( table3D, x, y, z, SIZE, SIZE ) += 1 ;
                            }
                            else
                            {
                                 //DEBUG_INT( VALUE16( TABLE2D( processedBitmap16->m_bitmap, x0, y0, SIZE ) ) )
                            }
                        }
                        else
                        {
                            //DEBUG( std::string( std::to_string( x0 ) + " : " + std::to_string( y0 ) ).c_str() );
                        }
                    }
                }
            }
	    }

		//DEBUG( std::to_string( ALPHA( angle ) ).c_str() );
	}

	printTable3D( table3D, SIZE, SIZE, SIZE );


	processedBitmap16->release();
	delete [] table3D;
	delete [] table2D;

	return 0;
}

/******************************************************************************/
