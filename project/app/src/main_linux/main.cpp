#include "common.hpp"
#include "core.hpp"
#include <string>
#include <string.h>
#include <cstdlib>
#include <cmath>
#include <vector>

using namespace COMMON;

long long COUNT = 0;    // number of bitmaps per full object rotation
long long TRUE_COUNT = 0;


/******************************************************************************/

bool isInCircle( UINT x, UINT y )
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

void generateCircle( BITMAP16_INNER_TYPE * table, size_t size )
{
	if( table == nullptr || size <= 0 )
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

bool isInOrb( UINT x, UINT y, UINT z )
{
	x -= SIZE / 2;
	y -= SIZE / 2;
	z -= SIZE / 2;
	x *= x;
	y *= y;
	z *= z;

	if( x + y + z <= ( SIZE / 2 ) * ( SIZE / 2 ) )
		return true;

   // DEBUG("fAIL");
	return false;
}

/******************************************************************************/

void generateOrb( CUBE16_INNER_TYPE * table, size_t size )
{
	if( table == nullptr || size <= 0 )
	{
		return;
	}

	UINT i, j, k;

	for( i = 0; i < size; i++ )
	{
		for( j = 0; j < size; j++ )
		{
		    for( k = 0; k < size; k++ )
		    {
                if( isInOrb( i, j, k ) )
                    TABLE3D( table, i, j, k, size, size ) = 1;
                else
                    TABLE3D( table, i, j, k, size, size ) = 0;
		    }
		}
	}

	return;
}

/******************************************************************************/

bool isInElipse( UINT x, UINT y, UINT a, UINT b )
{
	x -= SIZE / 2;
	y -= SIZE / 2;
	x *= x;
	y *= y;
	a *= a;
	b *= b;

	if( b * x + a * y <= a * b )
		return true;

	return false;
}

/******************************************************************************/

void generateElipse( BITMAP16_INNER_TYPE * table, size_t size, UINT a, UINT b )
{
	if( table == nullptr || size <= 0 )
	{
		return;
	}

	unsigned int i, j;

	for( i = 0; i < size; i++ )
	{
		for( j = 0; j < size; j++ )
		{
			if( isInElipse( i, j, a, b ) )
				TABLE2D( table, i, j, size ) = RGB16( 100, 100, 100 );
			else
				TABLE2D( table, i, j, size ) = RGB16( 255, 255, 255 );
		}
	}

	return;
}

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
				if( TABLE3D( table, i, j, k, sizeY, sizeZ ) == TRUE_COUNT )
					//fprintf( stdout, "%d,%d,%d%c%c", i, j, k, 13, 10 );
					fprintf( stdout, "%d %d %d\n", i, j, k );
			}
		}
	}

	return;
}

/******************************************************************************/

bool loadBitmap24FromFile( const char* path, BITMAP16_INNER_TYPE * table, size_t size )
{
	struct _BMPHeader
	{
		UINT16 type;
		UINT32 size;
		UINT16 reserved1;
		UINT16 reserved2;
		UINT32 offset;	// offset of pixel table in file
	} __attribute__((packed)) BMPHeader;
	
	FILE * pFile;
	UINT32 dwSize;
	

	pFile = fopen( path, "rb" );
  	if( pFile == nullptr )
	{
		raise_error( "loadBitmap24FromFile : Cannot open file" );
		return false;
	}

    fseek( pFile, 0, SEEK_END );
	dwSize = ftell( pFile );
	fseek( pFile, 0, SEEK_SET );

	if( dwSize < 14 )
	{
		raise_error( "loadBitmap24FromFile : File does not contain header" );
		return false;
	}

	if( fread( &BMPHeader, 1, SIZEOF( _BMPHeader ), pFile ) < SIZEOF( _BMPHeader ) )
	{
		raise_error( "loadBitmap24FromFile : Cannot read header from file" );
		return false;
	}

	UINT32 BitmapSize = BMPHeader.size - BMPHeader.offset;

	if( ( BitmapSize ) > 3 * size )
	{
		raise_error( "loadBitmap24FromFile : Provided buffer is too small" );
		return false;
	}

	struct Bitmap24InnerType
	{
		UINT8 blue;
		UINT8 green;
		UINT8 red;
	} *buffer;

	buffer = new Bitmap24InnerType[ BitmapSize / 3 ];
	if( buffer == nullptr )
	{
		raise_error( "loadBitmap24FromFile : Cannot allocate memory for bitmap" );
		return false;
	}

	fseek( pFile, BMPHeader.offset, SEEK_SET );
	if( fread( buffer, 1, BitmapSize, pFile ) < BitmapSize )
	{
		raise_error( "loadBitmap24FromFile : Cannot load pixel table" );
		return false;
	}

	// bitmap is inverted ( lines are written from bottom to top )
	for( UINT i = 0; i < BitmapSize / 3; i++ )
	{
		TABLE2D( table, ( i % SIZE ), ( SIZE - 1 - ( i / SIZE ) ) , SIZE ) = RGB16( buffer[i].red, buffer[i].green, buffer[i].blue );
	}

	delete [] buffer;
    fclose( pFile );

	return true;
}

/******************************************************************************/

#define ALPHA(q)    RAD( ( 360.0 * ( q ) ) / COUNT )
#define RAD(q)      ( PI * ( q ) / 180.0 )

int main( int argc, char * argv[] )
{
	
    if( argc < 2 || sscanf( argv[1], "%d", &COUNT ) < 1 || COUNT <= 0 )
    {
        raise_error( "main(): Invalid COUNT provided" );
    }
    TRUE_COUNT = COUNT;
	
	std::vector<PBitmap16> tables2D;
	char buffer[256];
	
	for( UINT i = 0; i < COUNT; i++ )
	{
		tables2D.emplace_back( new Bitmap16( PInnerBitmap16( new BITMAP16 ), SIZE, SIZE ) );
		memset( tables2D[i]->m_bitmap.get(), 0, BITMAP16_SIZE );
		sprintf( buffer, "./../resources/input/%u.bmp", i+1 );
		DEBUG(buffer)
		if( !loadBitmap24FromFile( buffer, tables2D[i]->m_bitmap.get(), BITMAP16_SIZE ) )
		{
			raise_error( "MAIN : Cannot load bitmap from file" );
		}
	}

	PCube16     table3D( new Cube16( PInnerCube16( new CUBE16 ), SIZE, SIZE, SIZE ) );  // 3D mesh
	CORE::Core  core;

	memset( table3D->m_cube.get(), 0, CUBE16_SIZE );

	//generateCircle( table2D->m_bitmap.get(), SIZE );
	//generateOrb( table3D->m_cube.get(), SIZE );
	//generateElipse( table2D->m_bitmap.get(), SIZE, SIZE / 8, SIZE/2 );

	//printTable2D( table2D->m_bitmap.get(), SIZE, SIZE );

    COMMON::STATUS result = core.Init16( table3D );
    if( result != COMMON::STATUS::OK )
    {
        raise_error( "Core->Init16: " + print_status( result ) );
    }

    for( int angle = 0; angle < COUNT; angle++ )
    {
        if( fabs( cos( angle ) ) < MIN_COS )
        {
            TRUE_COUNT--;
            continue;
        }

        result = core.ProvideBitmap16( tables2D[angle], ALPHA( angle ) );
        if( result != COMMON::STATUS::OK )
        {
            raise_error( "Core->ProvideBitmap16: " + print_status( result ) );
        }

        result = core.Start16( nullptr );
        if( result != COMMON::STATUS::OK )
        {
           raise_error( "Core->Start16: " + print_status( result ) );
        }

        core.ReleaseBitmap16();
    }

	printTable3D( table3D->m_cube.get(), SIZE, SIZE, SIZE );

    core.Destroy16();

	table3D.reset();
	tables2D.clear();

	return 0;
}

/******************************************************************************/
