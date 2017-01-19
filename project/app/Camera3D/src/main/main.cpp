#include <string>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <thread>
#include "common.hpp"
#include "core.hpp"
#include "xml_reader.hpp"

using namespace COMMON;

/******************************************************************************/

#define ALPHA(q)    RAD( ( 360.0 * ( q ) ) / g_count )
#define RAD(q)      ( PI * ( q ) / 180.0 )

#define XMLReaderPerform(fun) do { \
        XMLReader::Status XmlReaderResult = fun; \
        if( XmlReaderResult != XMLReader::Status::Ok ) \
        { \
        raise_error( "XMLReader: " #fun ": " + XMLReader::PrintStatus( XmlReaderResult ) ); \
        } \
    } while( 0 )

/******************************************************************************/

USHORT                  g_count = 0;                // number of bitmaps per full object rotation
LONGLONG                g_size = 0;                 // size of mesh = g_size x g_size x g_size
LONGLONG                g_distance = 0;             // distance from camera to screen
double                  g_cameraAngle = 0.0;        // camera's angle of vision
UINT                    g_numberOfThreads = 0;      // number of threads for processing
BYTE                    g_minBackgroundValue = 0;   // minimal value of background pixel (value = max(red, green, blue))
std::mutex*    		    g_pCubeMutexes;
CORE::Core*   			g_pCores;
std::vector<PBitmap16> 	g_bitmaps;

/******************************************************************************/

bool LoadBitmapFromFile( const char* path, BITMAP16_INNER_TYPE * table, size_t tableSize )
{
    PACK( struct _BMPHeader
    {
        UINT16    type;
        UINT32    size;
        UINT16    reserved1;
        UINT16    reserved2;
        UINT32    offset;	// offset of pixel table in file
    } ) BMPHeader;

    PACK( struct _BMPInfoHeader
    {
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
    } ) BMPInfoHeader;

    FILE * pFile;
    UINT32 fileSize;

    if( path == nullptr )
    {
        raise_error( "loadBitmapFromFile: Path cannot be nullptr" );
        return false;
    }

    if( table == nullptr )
    {
        raise_error( "loadBitmapFromFile: Table cannot be nullptr" );
        return false;
    }

    pFile = fopen( path, "rb" );
    if( pFile == nullptr )
    {
        raise_error( "loadBitmapFromFile: Cannot open file" );
        return false;
    }

    fseek( pFile, 0, SEEK_END );
    fileSize = ftell( pFile );
    fseek( pFile, 0, SEEK_SET );

    if( fileSize < SIZEOF( _BMPHeader ) )
    {
        raise_error( "loadBitmapFromFile: File does not contain header" );
        return false;
    }

    if( fread( &BMPHeader, 1, SIZEOF( _BMPHeader ), pFile ) < SIZEOF( _BMPHeader ) )
    {
        raise_error( "loadBitmapFromFile: Cannot read header from file" );
        return false;
    }

    UINT32 BitmapSize = BMPHeader.size - BMPHeader.offset;

    if( fread( &BMPInfoHeader, 1, SIZEOF( UINT32 ), pFile ) < SIZEOF( UINT32 ) )
    {
        raise_error( "loadBitmapFromFile: Cannot read Bitmap Info Header size from file" );
        return false;
    }

    if( BMPInfoHeader.biSize < 40 )
    {
        raise_error( "loadBitmapFromFile: Unsupported Bitmap Info Header in file" );
        return false;
    }

    if( fread( reinterpret_cast<char*>( &BMPInfoHeader ) + SIZEOF( UINT32 ), 1, ( SIZEOF( _BMPInfoHeader ) - SIZEOF( UINT32 ) ), pFile ) < ( SIZEOF( _BMPInfoHeader ) - SIZEOF( UINT32 ) ) )
    {
        raise_error( "loadBitmapFromFile: Cannot read Bitmap Info Header from file" );
        return false;
    }

    if( BMPInfoHeader.biWidth < SIZE || abs( static_cast<int>( BMPInfoHeader.biHeight ) ) != SIZE )
    {
        raise_error( "loadBitmapFromFile: Bitmap size IS NOT " + std::to_string( SIZE ) + "x" + std::to_string( SIZE ) );
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

    if( tableSize < BITMAP16_SIZE )
    {
        raise_error( "loadBitmapFromFile: Provided buffer is too small" );
        return false;
    }

    char * rawBuffer = new char[ BitmapSize ];
    if( rawBuffer == nullptr )
    {
        raise_error( "loadBitmapFromFile: Cannot allocate memory for bitmap" );
        return false;
    }

    fseek( pFile, BMPHeader.offset, SEEK_SET );
    if( fread( rawBuffer, 1, BitmapSize, pFile ) < BitmapSize )
    {
        delete[] rawBuffer;
        raise_error( "loadBitmapFromFile: Cannot load pixel table" );
        return false;
    }

    switch( BMPInfoHeader.biBitCount )
    {
        case 16:
        {
            BITMAP16_INNER_TYPE * buffer = reinterpret_cast<BITMAP16_INNER_TYPE *>( rawBuffer );

			UINT k = 0, i = 0;
            while( k < BitmapSize / 2 )
            {
            	// crop line to central part of SIZE width
            	if( k % BMPInfoHeader.biWidth < (BMPInfoHeader.biWidth - SIZE) / 2 ||
            		k % BMPInfoHeader.biWidth >= (BMPInfoHeader.biWidth - SIZE) / 2 + SIZE )
            	{
            		k++;
            		continue;
            	}
            
                // bitmap is inverted ( lines are written from bottom to top );
                // also X axis is switched with Y axis
                TABLE2D( table, ( i % SIZE ), ( SIZE - 1 - ( i / SIZE ) ), SIZE ) = buffer[ k ];
                i++;
                k++;
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

			UINT k = 0, i = 0;
            while( k < BitmapSize / 3 )
            {
            	// crop line to central part of SIZE width
            	if( k % BMPInfoHeader.biWidth < (BMPInfoHeader.biWidth - SIZE) / 2 ||
            		k % BMPInfoHeader.biWidth >= (BMPInfoHeader.biWidth - SIZE) / 2 + SIZE )
            	{
            		k++;
            		continue;
            	}
            
                // bitmap is inverted ( lines are written from bottom to top );
                // also X axis is switched with Y axis
                TABLE2D( table, ( i % SIZE ), ( SIZE - 1 - ( i / SIZE ) ), SIZE ) = RGB16( buffer[ k ].red, buffer[ k ].green, buffer[ k ].blue );
                i++;
                k++;
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

			UINT k = 0, i = 0;
            while( k < BitmapSize / 4 )
            {
            	// crop line to central part of SIZE width
            	if( k % BMPInfoHeader.biWidth < (BMPInfoHeader.biWidth - SIZE) / 2 ||
            		k % BMPInfoHeader.biWidth >= (BMPInfoHeader.biWidth - SIZE) / 2 + SIZE )
            	{
            		k++;
            		continue;
            	}
            
                // bitmap is inverted ( lines are written from bottom to top );
                // also X axis is switched with Y axis
                TABLE2D( table, ( i % SIZE ), ( SIZE - 1 - ( i / SIZE ) ), SIZE ) = RGB16( buffer[ k ].red, buffer[ k ].green, buffer[ k ].blue );
                i++;
                k++;
            }
            break;
        }
    } // switch

    delete[] rawBuffer;
    fclose( pFile );

    return true;
}

/******************************************************************************/

void NormalizeCube( PCube16& cube )
{
    if( !cube || !cube->isValid() )
    {
        raise_error( "NormalizeCube: Cube is not valid" );
    }

	CUBE16_INNER_TYPE * cubeTmp = cube->m_cube.get();

	int x = 0, y = 0, z = 0;
	for( x = 0; x < SIZE; x++ )
	{
		for( y = 0; y < SIZE; y++ )
		{
			for( z = 0; z < SIZE; z++ )
			{
				TABLE3D( cubeTmp, x, y, z, SIZE, SIZE ) = static_cast<CUBE16_INNER_TYPE>( TABLE3D( cubeTmp, x, y, z, SIZE, SIZE ) == g_count );
			}
		}
	}
	
	return;
}

/******************************************************************************/

void PreEnvelope( PCube16& cube )
{
    if( !cube || !cube->isValid() )
    {
        raise_error( "PreEnvelope: Cube is not valid" );
    }

	CUBE16_INNER_TYPE * cubeTmp = cube->m_cube.get();
	
	PCube16 neighboursMain( new Cube16( PInnerCube16( new CUBE16 ) ) );
	CUBE16_INNER_TYPE * neighbours = neighboursMain->m_cube.get();
	memset( neighbours, 0, CUBE16_SIZE );
	
	// count neighbours
	int x = 0, y = 0, z = 0;
	CUBE16_INNER_TYPE val = 0;
	
	for( x = 1; x < SIZE - 1; x++ )
	{
		for( y = 1; y < SIZE - 1; y++ )
		{
			for( z = 1; z < SIZE - 1; z++ )
			{
				val = TABLE3D( cubeTmp, x, y, z, SIZE, SIZE );
				
				TABLE3D( neighbours, x-1, y-1, z-1, SIZE, SIZE ) += val;
				TABLE3D( neighbours, x-1, y-1, z  , SIZE, SIZE ) += val;
				TABLE3D( neighbours, x-1, y-1, z+1, SIZE, SIZE ) += val;
				TABLE3D( neighbours, x-1, y  , z-1, SIZE, SIZE ) += val;
				TABLE3D( neighbours, x-1, y  , z  , SIZE, SIZE ) += val;
				TABLE3D( neighbours, x-1, y  , z+1, SIZE, SIZE ) += val;
				TABLE3D( neighbours, x-1, y+1, z-1, SIZE, SIZE ) += val;
				TABLE3D( neighbours, x-1, y+1, z  , SIZE, SIZE ) += val;
				TABLE3D( neighbours, x-1, y+1, z+1, SIZE, SIZE ) += val;
				
				TABLE3D( neighbours, x  , y-1, z-1, SIZE, SIZE ) += val;
				TABLE3D( neighbours, x  , y-1, z  , SIZE, SIZE ) += val;
				TABLE3D( neighbours, x  , y-1, z+1, SIZE, SIZE ) += val;
				TABLE3D( neighbours, x  , y  , z-1, SIZE, SIZE ) += val;
				TABLE3D( neighbours, x  , y  , z+1, SIZE, SIZE ) += val;
				TABLE3D( neighbours, x  , y+1, z-1, SIZE, SIZE ) += val;
				TABLE3D( neighbours, x  , y+1, z  , SIZE, SIZE ) += val;
				TABLE3D( neighbours, x  , y+1, z+1, SIZE, SIZE ) += val;
				
				TABLE3D( neighbours, x+1, y-1, z-1, SIZE, SIZE ) += val;
				TABLE3D( neighbours, x+1, y-1, z  , SIZE, SIZE ) += val;
				TABLE3D( neighbours, x+1, y-1, z+1, SIZE, SIZE ) += val;
				TABLE3D( neighbours, x+1, y  , z-1, SIZE, SIZE ) += val;
				TABLE3D( neighbours, x+1, y  , z  , SIZE, SIZE ) += val;
				TABLE3D( neighbours, x+1, y  , z+1, SIZE, SIZE ) += val;
				TABLE3D( neighbours, x+1, y+1, z-1, SIZE, SIZE ) += val;
				TABLE3D( neighbours, x+1, y+1, z  , SIZE, SIZE ) += val;
				TABLE3D( neighbours, x+1, y+1, z+1, SIZE, SIZE ) += val;
			}
		}
	}
	
	// filter points that have more or exactly 18 neighbours
	for( x = 1; x < SIZE - 1; x++ )
	{
		for( y = 1; y < SIZE - 1; y++ )
		{
			for( z = 1; z < SIZE - 1; z++ )
			{
				if( TABLE3D( cubeTmp, x, y, z, SIZE, SIZE ) )
				{
					TABLE3D( cubeTmp, x, y, z, SIZE, SIZE ) = static_cast<CUBE16_INNER_TYPE>( TABLE3D( neighbours, x, y, z, SIZE, SIZE ) < 18 );
				}
			}
		}
	}
	
	return;
}

void SaveMeshToFile( const std::string& filePath, const PCube16& cube, size_t size = SIZE )
{
    BITMAP16_INNER_TYPE * table = cube->m_cube.get();

    if( cube == nullptr || size <= 0 )
    {
        return;
    }

    FILE * pFile = nullptr;
    int i, j, k;

    pFile = fopen( filePath.c_str(), "wb" );
    if( pFile == nullptr )
    {
        raise_error( "SaveMeshToFile: Cannot create/open output file" );
    }

    for( i = 0; i < static_cast<int>( size ); i++ )
    {
        for( j = 0; j < static_cast<int>( size ); j++ )
        {
            for( k = 0; k < static_cast<int>( size ); k++ )
            {
                if( TABLE3D( table, i, j, k, size, size ) )
                    // configured for gnuplot
                    fprintf( pFile, "%d %d %d\n", i, k, static_cast<int>( size ) - j - 1 );
            }
        }
    }

    fclose( pFile );
    return;
}

/******************************************************************************/

void DispatchCores( UINT core_num )
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

        if( i >= g_count )
        {
            break;
        }

        result = g_pCores[ core_num ].ProvideBitmap16( g_bitmaps[ i ], ALPHA( i ) );
        if( result != COMMON::STATUS::OK )
        {
            raise_error( "Core->ProvideBitmap16: " + print_status( result ) );
        }

        result = g_pCores[ core_num ].Start16( nullptr );
        if( result != COMMON::STATUS::OK )
        {
            raise_error( "Core->Start16: " + print_status( result ) );
        }

        g_pCores[ core_num ].ReleaseBitmap16();
    }

    return;
}

/******************************************************************************/

int main( int argc, char * argv[] )
{
    std::string configXmlPath = DEFAULT_CONFIG_XML;

    // parse input arguments
    if( argc < 2 || ( argc >= 2 && strcmp( argv[ 1 ], "" ) == 0 ) )
    {
        fprintf( stderr, "No path to configuration file provided - defaulting to:\n'" DEFAULT_CONFIG_XML "'\n" );
    }
    else
    {
        configXmlPath.assign( argv[ 1 ] );
    }

    // load configuration xml file
    XMLReader XmlReader;
    XMLReaderPerform( XmlReader.LoadFile( configXmlPath, "CloudComputing" ) );
    DEBUG( "Config XML file loaded" );

    // get parameter 'INPUT_PATH'
    std::string inputPath = "";
    XMLReaderPerform( XmlReader.GetParameter( "INPUT_PATH", inputPath ) );
    DEBUG( "'INPUT_PATH' parameter loaded" );

    // get parameter 'OUTPUT_FILE'
    std::string outputFile = "";
    XMLReaderPerform( XmlReader.GetParameter( "OUTPUT_FILE", outputFile ) );
    DEBUG( "'OUTPUT_FILE' parameter loaded" );

    // check whether 'OUTPUT_FILE' parameter is valid
    FILE * testFile = fopen( outputFile.c_str(), "wb" );
    if( testFile == nullptr )
    {
        raise_error( "MAIN: Cannot create/open output file" );
    }
    fclose( testFile );

    // get parameter 'SIZE'
    XMLReaderPerform( XmlReader.GetParameter( "SIZE", g_size ) );
    DEBUG( "'SIZE' parameter loaded" );
    if( g_size <= 0 )
    {
        raise_error( "MAIN: Invalid 'SIZE' parameter - must be greater than 0" );
    }

    // get parameter 'COUNT'
    XMLReaderPerform( XmlReader.GetParameter( "COUNT", g_count ) );
    DEBUG( "'COUNT' parameter loaded" );
    if( g_count == 0 )
    {
        raise_error( "MAIN: Invalid 'COUNT' parameter - must be greater than 0" );
    }

    // get parameter 'CYCLIC_LOAD'
    bool cyclicLoad = false;
    XMLReaderPerform( XmlReader.GetParameter( "CYCLIC_LOAD", cyclicLoad ) );
    DEBUG( "'CYCLIC_LOAD' parameter loaded" );

    // get parameter 'DISTANCE'
    XMLReaderPerform( XmlReader.GetParameter( "DISTANCE", g_distance ) );
    DEBUG( "'DISTANCE' parameter loaded" );
    if( g_distance <= 0 )
    {
        raise_error( "MAIN: Invalid 'DISTANCE' parameter - must be greater than 0" );
    }

    // get parameter 'CAMERA_ANGLE'
    XMLReaderPerform( XmlReader.GetParameter( "CAMERA_ANGLE", g_cameraAngle ) );
    DEBUG( "'CAMERA_ANGLE' parameter loaded" );
    if( g_cameraAngle <= 0.0 )
    {
        raise_error( "MAIN: Invalid 'CAMERA_ANGLE' parameter - must be greater than 0.0" );
    }

    // get parameter 'MIN_BACKGROUND_VALUE'
    XMLReaderPerform( XmlReader.GetParameter( "MIN_BACKGROUND_VALUE", g_minBackgroundValue ) );
    DEBUG( "'MIN_BACKGROUND_VALUE' parameter loaded" );

    // get parameter 'NUM_THREADS'
    XMLReaderPerform( XmlReader.GetParameter( "NUM_THREADS", g_numberOfThreads ) );
    DEBUG( "'NUM_THREADS' parameter loaded" );
    if( g_numberOfThreads == 0 )
    {
        raise_error( "MAIN: Invalid 'NUM_THREADS' parameter - must be greater than 0" );
    }

    // clear XmlReader content
    XmlReader.Clear();

    // create global table of mutexes
    g_pCubeMutexes = new std::mutex[ g_numberOfThreads ];
    if( g_pCubeMutexes == nullptr )
    {
        raise_error( "MAIN: Cannot allocate memory for cube mutexes" );
    }

    // create global table of processing cores
    g_pCores = new CORE::Core[ g_numberOfThreads ];
    if( g_pCores == nullptr )
    {
        raise_error( "MAIN: Cannot allocate memory for cores" );
    }

    // load bitmaps from files
    char buffer[ 256 ];
    for( UINT i = 0; i < g_count; i++ )
    {
        PBitmap16 bitmap16( new Bitmap16( PInnerBitmap16( new BITMAP16 ) ) );
        g_bitmaps.push_back( bitmap16 );

        memset( g_bitmaps[ i ]->m_bitmap.get(), 0, BITMAP16_SIZE );

#ifdef _MSC_VER
        sprintf( buffer, "%s\\%u.bmp", inputPath.c_str(), IF( cyclicLoad, 1, i + 1 ) );
#else
        sprintf( buffer, "%s/%u.bmp", inputPath.c_str(), IF( cyclicLoad, 1, i + 1 ) );
#endif
        
        DEBUG( buffer );
        if( !LoadBitmapFromFile( buffer, g_bitmaps[ i ]->m_bitmap.get(), BITMAP16_SIZE ) )
        {
            raise_error( "MAIN: Cannot load bitmap from file" );
        }
        DEBUG( std::string( "Loading completed: " + std::to_string( i + 1 ) + "/" + std::to_string( g_count ) ).c_str() );
    }

    // reserve memory for mesh
    PCube16 table3D( new Cube16( PInnerCube16( new CUBE16 ) ) );  // 3D mesh
    memset( table3D->m_cube.get(), 0, CUBE16_SIZE );

    // initialize processing cores
    COMMON::STATUS result = COMMON::STATUS::OK;
    for( UINT i = 0; i < g_numberOfThreads; i++ )
    {
        result = g_pCores[ i ].Init16( table3D );
        if( result != COMMON::STATUS::OK )
        {
            raise_error( "Core->Init16: " + print_status( result ) );
        }
    }

    // start processing
    std::shared_ptr<std::thread> threads( new std::thread[ g_numberOfThreads ] );
    for( UINT i = 0; i < g_numberOfThreads; i++ )
    {
        threads.get()[ i ] = std::thread( DispatchCores, i );
    }

    // wait for processing to finish
    for( UINT i = 0; i < g_numberOfThreads; i++ )
    {
        threads.get()[ i ].join();
        g_pCores[ i ].Destroy16();
    }

    // post computation actions
    NormalizeCube( table3D );
    PreEnvelope( table3D );

    // save mesh to file
    SaveMeshToFile( outputFile, table3D );

    // clear resources
    table3D.reset();
    g_bitmaps.clear();
    delete[] g_pCores;
    delete[] g_pCubeMutexes;

    // exit
    COMMON::exit( 0 );
    return 0;
}

/******************************************************************************/
