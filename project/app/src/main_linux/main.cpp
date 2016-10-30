#include "common.hpp"
#include "core.hpp"
#include <string>
#include <string.h>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <thread>
#include <mutex>

using namespace COMMON;

/******************************************************************************/

#define ALPHA(q)    RAD( ( 360.0 * ( q ) ) / g_COUNT )
#define RAD(q)      ( PI * ( q ) / 180.0 )

/******************************************************************************/

long long 				g_COUNT = 0;    // number of bitmaps per full object rotation
//long long 			g_TRUE_COUNT = 0;
std::mutex 				g_cubeMutex[NUM_THREADS];
CORE::Core  			g_cores[NUM_THREADS];
std::vector<PBitmap16> 	g_bitmaps;

/******************************************************************************/

void printTable2D( BITMAP16_INNER_TYPE * table, size_t sizeX, size_t sizeY )
{
	if( table == nullptr || sizeX <= 0 || sizeY <= 0 )
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

void printTable3D( CUBE16_INNER_TYPE * table, size_t sizeX, size_t sizeY, size_t sizeZ )
{
    //DEBUG("kiełbasa");

	if( table == nullptr || sizeX <= 0 || sizeY <= 0 || sizeZ <= 0 )
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
			    //DEBUG_INT(TABLE3D( table, i, j, k, sizeY, sizeZ ));
				if( TABLE3D( table, i, j, k, sizeY, sizeZ ) == g_COUNT )
					// configured for gnuplot
					fprintf( stdout, "%d %d %d\n", i, k, sizeY - j - 1 );
			}
		}
	}

	return;
}

/******************************************************************************/

bool loadBitmapFromFile( const char* path, BITMAP16_INNER_TYPE * table, size_t size )
{
	struct _BMPHeader
	{
		UINT16    type;
		UINT32    size;
		UINT16    reserved1;
		UINT16    reserved2;
		UINT32    offset;	// offset of pixel table in file
	} __attribute__((packed)) BMPHeader;

	struct _BMPInfoHeader {
		UINT32    biSize;
		UINT32    biWidth;
		UINT32    biHeight;
		UINT16    biPlanes;
		UINT16    biBitCount;
		UINT32    biCompression;
		UINT32    biSizeImage;
		UINT32    biXPelsPerMeter;
		UINT32    biYPelsPerMeter;
		UINT32    biClrUsed;
		UINT32    biClrImportant;
	} __attribute__((packed)) BMPInfoHeader;
	
	FILE * pFile;
	UINT32 dwSize;
	

	pFile = fopen( path, "rb" );
  	if( pFile == nullptr )
	{
		raise_error( "loadBitmapFromFile : Cannot open file" );
		return false;
	}

    fseek( pFile, 0, SEEK_END );
	dwSize = ftell( pFile );
	fseek( pFile, 0, SEEK_SET );

	if( dwSize < 14 )
	{
		raise_error( "loadBitmapFromFile : File does not contain header" );
		return false;
	}

	if( fread( &BMPHeader, 1, SIZEOF( _BMPHeader ), pFile ) < SIZEOF( _BMPHeader ) )
	{
		raise_error( "loadBitmapFromFile : Cannot read header from file" );
		return false;
	}

	UINT32 BitmapSize = BMPHeader.size - BMPHeader.offset;

	if( fread( &BMPInfoHeader, 1, SIZEOF( UINT32 ), pFile ) < SIZEOF( UINT32 ) )
	{
		raise_error( "loadBitmapFromFile : Cannot read Bitmap Info Header size from file" );
		return false;
	}

	if( BMPInfoHeader.biSize < 40 )
	{
		raise_error( "loadBitmapFromFile : Unsupported Bitmap Info Header in file" );
		return false;
	}

	if( fread( reinterpret_cast<char*>( &BMPInfoHeader ) + SIZEOF( UINT32 ), 1, ( SIZEOF( _BMPInfoHeader ) - SIZEOF( UINT32 ) ), pFile ) < ( SIZEOF( _BMPInfoHeader ) - SIZEOF( UINT32 ) ) )
	{
		raise_error( "loadBitmapFromFile : Cannot read Bitmap Info Header from file" );
		return false;
	}

	if( BMPInfoHeader.biWidth != SIZE || abs( BMPInfoHeader.biHeight ) != SIZE )
	{
		raise_error( "loadBitmapFromFile : Bitmap size IS NOT " + std::to_string( SIZE ) + "x" + std::to_string( SIZE ) );
		return false;
	}

	switch( BMPInfoHeader.biBitCount )
	{
		case 16:
		case 24:
		case 32:
			break;
		default:
			raise_error( "loadBitmapFromFile: Only 16-bit, 24-bit and 32-bit bitmaps can be loaded" );
			return false;
	}

	if( SIZEOF( BITMAP16_INNER_TYPE ) * ( BitmapSize ) > BMPInfoHeader.biBitCount / 8 * size )
	{
		raise_error( "loadBitmapFromFile : Provided buffer is too small" );
		return false;
	}

	char * rawBuffer = new char[ BitmapSize ];
	if( rawBuffer == nullptr )
	{
		raise_error( "loadBitmapFromFile : Cannot allocate memory for bitmap" );
		return false;
	}

	fseek( pFile, BMPHeader.offset, SEEK_SET );
	if( fread( rawBuffer, 1, BitmapSize, pFile ) < BitmapSize )
	{
		raise_error( "loadBitmapFromFile : Cannot load pixel table" );
		return false;
	}

	switch( BMPInfoHeader.biBitCount )
	{
		case 16:
			{
				BITMAP16_INNER_TYPE * buffer = reinterpret_cast<BITMAP16_INNER_TYPE *>( rawBuffer );

				for( UINT i = 0; i < BitmapSize / 2; i++ )
				{
					// bitmap is inverted ( lines are written from bottom to top );
					// also X axis is switched with Y axis
					TABLE2D( table, ( i % SIZE ), ( SIZE - 1 - ( i / SIZE ) ) , SIZE ) = buffer[i];
				}
				break;
			}
		case 24:
			{
				typedef struct
				{
					UINT8 blue;
					UINT8 green;
					UINT8 red;
				} BITMAP24_ELEMENT;

				BITMAP24_ELEMENT * buffer = reinterpret_cast<BITMAP24_ELEMENT*>( rawBuffer );

				for( UINT i = 0; i < BitmapSize / 3; i++ )
				{
					// bitmap is inverted ( lines are written from bottom to top );
					// also X axis is switched with Y axis
					TABLE2D( table, ( i % SIZE ), ( SIZE - 1 - ( i / SIZE ) ) , SIZE ) = RGB16( buffer[i].red, buffer[i].green, buffer[i].blue );
				}
				break;
			}
		case 32:
			{
				typedef struct
				{
					UINT8 alpha;
					UINT8 blue;
					UINT8 green;
					UINT8 red;
				} BITMAP32_ELEMENT;

				BITMAP32_ELEMENT * buffer = reinterpret_cast<BITMAP32_ELEMENT*>( rawBuffer );

				for( UINT i = 0; i < BitmapSize / 4; i++ )
				{
					// bitmap is inverted ( lines are written from bottom to top );
					// also X axis is switched with Y axis
					TABLE2D( table, ( i % SIZE ), ( SIZE - 1 - ( i / SIZE ) ) , SIZE ) = RGB16( buffer[i].red, buffer[i].green, buffer[i].blue );
				}
				break;
			}
	} // switch

	delete [] rawBuffer;
    fclose( pFile );

	return true;
}

/******************************************************************************/

void dispatchCores( UINT core_num )
{
	static UINT nextRound( 0 );
	static std::mutex mutex_nextRound;

	COMMON::STATUS result = COMMON::STATUS::OK;

	while( true )
	{
		mutex_nextRound.lock();
		int i = nextRound;
		nextRound++;
		mutex_nextRound.unlock();

		if( i >= g_COUNT )
		{
			break;
		}

		result = g_cores[core_num].ProvideBitmap16( g_bitmaps[i], ALPHA( i ) );
		if( result != COMMON::STATUS::OK )
		{
			raise_error( "Core->ProvideBitmap16: " + print_status( result ) );
		}

		result = g_cores[core_num].Start16( nullptr );
		if( result != COMMON::STATUS::OK )
		{
			raise_error( "Core->Start16: " + print_status( result ) );
		}

		g_cores[core_num].ReleaseBitmap16();
	}

	return;
}

/******************************************************************************/

int main( int argc, char * argv[] )
{
	bool cyclicLoad = false;

    if( argc < 2 || sscanf( argv[1], "%d", &g_COUNT ) < 1 || g_COUNT <= 0 )
    {
        raise_error( "main(): Invalid g_COUNT provided" );
    }
	if( argc == 3 && strcmp( argv[2], "1" ) == 0 )
	{
		cyclicLoad = true;
	}
    //g_TRUE_COUNT = g_COUNT;
	
	//g_bitmaps.reserve(16);
	char buffer[256];
	
	for( UINT i = 0; i < g_COUNT; i++ )
	{
		PBitmap16 bitmap16( new Bitmap16( PInnerBitmap16( new BITMAP16 ), SIZE, SIZE ) );
		g_bitmaps.push_back( bitmap16 );

		memset( g_bitmaps[i]->m_bitmap.get(), 0, BITMAP16_SIZE );
		sprintf( buffer, "./../resources/input/%u.bmp", IF( cyclicLoad, 1, i+1 ) );
		DEBUG( buffer )
		if( !loadBitmapFromFile( buffer, g_bitmaps[i]->m_bitmap.get(), BITMAP16_SIZE ) )
		{
			raise_error( "MAIN : Cannot load bitmap from file" );
		}
		DEBUG( "Loading completed" )
	}

	PCube16     table3D( new Cube16( PInnerCube16( new CUBE16 ), SIZE, SIZE, SIZE ) );  // 3D mesh

	memset( table3D->m_cube.get(), 0, CUBE16_SIZE );

	//generateCircle( table2D->m_bitmap.get(), SIZE );
	//generateOrb( table3D->m_cube.get(), SIZE );
	//generateElipse( table2D->m_bitmap.get(), SIZE, SIZE / 8, SIZE/2 );

	//printTable2D( table2D->m_bitmap.get(), SIZE, SIZE );

    COMMON::STATUS result = COMMON::STATUS::OK;
	for( UINT i = 0; i < NUM_THREADS; i++ )
	{
		result = g_cores[i].Init16( table3D );
		if( result != COMMON::STATUS::OK )
		{
			raise_error( "Core->Init16: " + print_status( result ) );
		}
	}

	std::thread threads[NUM_THREADS];
	for( UINT i = 0; i < NUM_THREADS; i++ )
	{
		threads[i] = std::thread( dispatchCores, i );
	}

	for( UINT i = 0; i < NUM_THREADS; i++ )
	{
		threads[i].join();
		g_cores[i].Destroy16();
	}

	printTable3D( table3D->m_cube.get(), SIZE, SIZE, SIZE );

	table3D.reset();
	g_bitmaps.clear();

	return 0;
}

/******************************************************************************/
