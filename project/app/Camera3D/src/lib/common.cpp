#include <cerrno>
#include <cstdlib>
#include "common.hpp"

void COMMON::exit( int error_code )
{
    if( error_code )
    {
        system( "pause" );
    }
    ::exit( error_code );
}

/******************************************************************************/

void COMMON::raise_error( const char * str )
{
#ifndef _MSC_VER
    fprintf( stderr, "\x1b[1;31m" );
#endif
    fprintf( stderr, "Error: %s\n", str );
#ifndef _MSC_VER
    fprintf( stderr, "\x1b[0m" );
#endif
    fflush( stderr );
    COMMON::exit( 1 );
}

/******************************************************************************/

void COMMON::raise_error( const std::string str )
{
#ifndef _MSC_VER
    fprintf( stderr, "\x1b[1;31m" );
#endif
    fprintf( stderr, "Error: %s\n", str.c_str() );
#ifndef _MSC_VER
    fprintf( stderr, "\x1b[0m" );
#endif
    fflush( stderr );
    COMMON::exit( 1 );
}

/******************************************************************************/

void COMMON::raise_perror( const char * str )
{
#ifndef _MSC_VER
    fprintf( stderr, "\x1b[1;31m" );
#endif
    fprintf( stderr, "Error: %s\n", str );
    perror( "Error" );
#ifndef _MSC_VER
    fprintf( stderr, "\x1b[0m" );
#endif
    fflush( stderr );
    COMMON::exit( 1 );
}

/******************************************************************************/

void COMMON::raise_perror( const std::string str )
{
#ifndef _MSC_VER
    fprintf( stderr, "\x1b[1;31m" );
#endif
    fprintf( stderr, "Error: %s\n", str.c_str() );
    perror( "Error" );
#ifndef _MSC_VER
    fprintf( stderr, "\x1b[0m" );
#endif
    fflush( stderr );
    COMMON::exit( 1 );
}


/******************************************************************************/

COMMON::Bitmap16::Bitmap16( COMMON::PInnerBitmap16 bitmap )
    : m_bitmap( bitmap )
{
}

/******************************************************************************/

COMMON::Bitmap16::~Bitmap16()
{
}

/******************************************************************************/

bool COMMON::Bitmap16::isValid()
{
    if( m_bitmap && SIZE > 0 )
    {
        return true;
    }

    return false;
}

/******************************************************************************/

COMMON::Cube16::Cube16( COMMON::PInnerCube16 cube )
    : m_cube( cube )
{
}

/******************************************************************************/

COMMON::Cube16::~Cube16()
{
}

/******************************************************************************/

bool COMMON::Cube16::isValid()
{
    if( m_cube && SIZE > 0 )
    {
        return true;
    }

    return false;
}

/******************************************************************************/

std::string COMMON::print_status( STATUS status )
{
    switch( status )
    {
        case COMMON::STATUS::OK:                    return "OK";
        case COMMON::STATUS::INVALID_BITMAP:        return "Invalid bitmap provided";
        case COMMON::STATUS::INVALID_CUBE:          return "Invalid cube provided";
        case COMMON::STATUS::NOT_INITIALIZED:       return "Object not initialized";
        case COMMON::STATUS::ALREADY_INITIALIZED:   return "Object already initialized";
        default:                                    return "Unknown error";
    }
}

/******************************************************************************/

void COMMON::printTable2D( BITMAP16_INNER_TYPE * table, size_t size )
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
            fprintf( stdout, "%4u", VALUE16( TABLE2D( table, j, i, size ) ) );
        }

        fprintf( stdout, "\n" );
    }

    return;
}

/******************************************************************************/

void COMMON::printTable3D( CUBE16_INNER_TYPE * table, size_t size )
{
    if( table == nullptr || size <= 0 )
    {
        return;
    }

    int i, j, k;

    for( i = 0; i < static_cast<int>( size ); i++ )
    {
        for( j = 0; j < static_cast<int>( size ); j++ )
        {
            for( k = 0; k < static_cast<int>( size ); k++ )
            {
                if( TABLE3D( table, i, j, k, size, size ) )
                    // configured for gnuplot
                    fprintf( stdout, "%d %d %d\n", i, k, static_cast<int>( size ) - j - 1 );
            }
        }
    }

    return;
}

/******************************************************************************/

