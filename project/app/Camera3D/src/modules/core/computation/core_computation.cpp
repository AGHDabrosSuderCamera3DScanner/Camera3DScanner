#include <cmath>
#include <cstring>
#include "core_computation.hpp"

/******************************************************************************/

CORE::Computation::Computation()
    : m_tempValues( nullptr )
    , m_initialized16( false )
    , m_cube16( nullptr )
{
}

/******************************************************************************/

CORE::Computation::~Computation()
{
}

/******************************************************************************/

COMMON::STATUS CORE::Computation::Init16( COMMON::PCube16& pCube16 )
{
    if( m_initialized16 )
    {
        return COMMON::STATUS::ALREADY_INITIALIZED;
    }

    if( !pCube16 || !pCube16->isValid() )
    {
        return COMMON::STATUS::INVALID_CUBE;
    }

    m_tempValues = new UINT8[ static_cast<size_t>( SIZE * SIZE ) ];
    m_cube16 = pCube16;
    m_initialized16 = true;
    return COMMON::STATUS::OK;
}

/******************************************************************************/

COMMON::STATUS CORE::Computation::Compute16( COMMON::PBitmap16& pBitmap16, double angle )
{
    if( !m_initialized16 )
    {
        return COMMON::STATUS::NOT_INITIALIZED;
    }

    if( !pBitmap16 || !pBitmap16->isValid() )
    {
        return COMMON::STATUS::INVALID_BITMAP;
    }

    BITMAP16_INNER_TYPE * bitmap = pBitmap16->m_bitmap.get();
    CUBE16_INNER_TYPE * cube = m_cube16->m_cube.get();
    const LONGLONG size = SIZE;
    const LONGLONG l = L;
    const double camera_angle = CAMERA_ANGLE;

#undef SIZE
#define SIZE size
#undef SIZE_2
#define SIZE_2 ( static_cast<LONGLONG>( size / 2ULL ) )
#undef L
#define L l
#undef CAMERA_ANGLE
#define CAMERA_ANGLE camera_angle

#define SCREEN      ( ( 2ULL * L ) * tan( CAMERA_ANGLE / 2 ) )

#define COS(q) (cos( q ))
#define SIN(q) (sin( q ))
#define TAN(q) (tan( q ))
#define beta ( angle )

#define h ( SHIFT( x ) )
#define g ( h * TAN( beta ) )
#define f ( h / COS( beta ) )
#define j ( ( SIZE_2 - z ) - g )
#define e ( j * SIN( beta ) )
#define c ( j * COS( beta ) )
#define b ( ( SIZE_2 - c ) )
#define a ( D )

    int x0, y0, x, y, z;
    
    //UINT8 tempValues[ SIZE ][ SIZE ];
    memset( m_tempValues, 0, SIZE * SIZE * SIZEOF( UINT8 ) );

    for( x = 0; x < SIZE; x++ )
    {
        for( z = 0; z < SIZE; z++ )
        {
            //x0 = ( L * SHIFT( x ) ) / ( D + z ); // bez kąta
            x0 = static_cast<int>( ( L * ( e + f ) / ( a + b ) ) * SIZE / SCREEN + SIZE_2 );
            if( x0 < SIZE && x0 >= 0 )
            {
                for( y = 0; y < SIZE; y++ )
                {
                    //y0 = ( L * SHIFT( y ) ) / ( D + z ); // bez kąta
#define result ( ( SIZE_2 - static_cast<double>(z) ) * COS( beta ) + SHIFT( x ) * SIN( beta ) + SIZE_2 )

                    y0 = static_cast<int>( ( SHIFT( y ) * static_cast<double>( L ) / ( L - result ) ) * SIZE / SCREEN + SIZE_2 );
                    if( y0 < SIZE && y0 >= 0 )
                    {
                        if( TABLE2D( bitmap, x0, y0, SIZE ) == 0 )
                        {
                            m_tempValues[ y * SIZE + z ] = 1;
                        }
                    }
                } // for
            } // if
        } // for

        g_pCubeMutexes[ g_numberOfThreads * x / SIZE ].lock();
        for( y = 0; y < SIZE; y++ )
        {
            for( z = 0; z < SIZE; z++ )
            {
                TABLE3D( cube, x, y, z, SIZE, SIZE ) += m_tempValues[ y * SIZE + z ];
                m_tempValues[ y * SIZE + z ] = 0;
            }
        }
        g_pCubeMutexes[ g_numberOfThreads * x / SIZE ].unlock();
    } // for

    //delete[] tempValues;

#undef CAMERA_ANGLE
#define CAMERA_ANGLE g_cameraAngle
#undef L
#define L g_distance
#undef SIZE_2
#define SIZE_2 ( SIZE / 2 )
#undef SIZE
#define SIZE g_size

    return COMMON::STATUS::OK;
}

/******************************************************************************/

void CORE::Computation::Destroy16()
{
    m_initialized16 = false;
    delete [] m_tempValues;
    m_cube16 = nullptr;
}

/******************************************************************************/
