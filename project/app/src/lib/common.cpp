#include "common.hpp"
#include <errno.h>
#include <stdlib.h>

/******************************************************************************/

void COMMON::raise_error( const char * str )
{
    fprintf(stderr, "\x1b[1;31m");
	fprintf(stderr, "Error: %s\n", str);
	fprintf(stderr, "\x1b[0m");
	fflush(stderr);
	exit(1);
}

/******************************************************************************/

void COMMON::raise_error( const std::string str )
{
    fprintf(stderr, "\x1b[1;31m");
	fprintf(stderr, "Error: %s\n", str.c_str());
	fprintf(stderr, "\x1b[0m");
	fflush(stderr);
	exit(1);
}

/******************************************************************************/

void COMMON::raise_perror( const char * str )
{
    fprintf(stderr, "\x1b[1;31m");
	fprintf(stderr, "Error: %s\n", str);
	perror("Error");
	fprintf(stderr, "\x1b[0m");
	fflush(stderr);
	exit(1);
}

/******************************************************************************/

void COMMON::raise_perror( const std::string str )
{
    fprintf(stderr, "\x1b[1;31m");
	fprintf(stderr, "Error: %s\n", str.c_str());
	perror("Error");
	fprintf(stderr, "\x1b[0m");
	fflush(stderr);
	exit(1);
}


/******************************************************************************/

COMMON::Bitmap16::Bitmap16(
    COMMON::PInnerBitmap16 bitmap,
    size_t sizeX,
    size_t sizeY )
    :
    m_bitmap( bitmap ),
    m_sizeX( sizeX ),
    m_sizeY( sizeY )
{
}

/******************************************************************************/

COMMON::Bitmap16::~Bitmap16()
{
}

/******************************************************************************/

bool COMMON::Bitmap16::isValid()
{
	if( ( m_bitmap ) && ( m_sizeX > 0 ) && ( m_sizeY > 0 ) )
	{
		return true;
	}

	return false;
}

/******************************************************************************/

COMMON::Cube16::Cube16(
    COMMON::PInnerCube16 cube,
    size_t sizeX,
    size_t sizeY,
    size_t sizeZ )
    :
    m_cube( cube ),
    m_sizeX( sizeX ),
    m_sizeY( sizeY ),
    m_sizeZ( sizeZ )
{
}

/******************************************************************************/

COMMON::Cube16::~Cube16()
{
}

/******************************************************************************/

bool COMMON::Cube16::isValid()
{
	if( ( m_cube ) && ( m_sizeX > 0 ) && ( m_sizeY > 0 ) && ( m_sizeZ > 0 ) )
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
