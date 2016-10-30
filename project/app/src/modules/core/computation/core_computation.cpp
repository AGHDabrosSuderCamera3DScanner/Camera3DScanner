#include "core_computation.hpp"
#include <cmath>
#include <cstring>

/******************************************************************************/

CORE::Computation::Computation() :
    m_cube16( nullptr ),
    m_initialized16( false )
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

    //#define SCREEN      ( ( ( L * L * SIZE ) / ( 2.0 * L - SIZE ) ) / static_cast<double>( sqrt( L * L - pow( ( L * SIZE ) / ( 2.0 * L - SIZE ) , 2 ) ) ) )
    //define SCREEN      ( SIZE * static_cast<double>( sqrt( ( 1 + sin( CAMERA_ANGLE / 2 ) ) / ( 1 - sin( CAMERA_ANGLE / 2 ) ) ) ) )
    #define SCREEN      ( 2 * L * tan( CAMERA_ANGLE / 2 ) )

    #define COS(q) (cos( q ))
    #define SIN(q) (sin( q ))
    #define TAN(q) (tan( q ))
    //#define beta ( ALPHA( angle ) )
    #define beta ( angle )

    #define h ( ( SHIFT( x ) ) )
    #define g ( h * TAN( beta ) )
    #define f ( h / COS( beta ) )
    #define j ( ( SIZE_2 - z ) - g )
    #define e ( j * SIN( beta ) )
    #define c ( j * COS( beta ) )
    #define b ( ( SIZE_2 - c ) )
    #define a ( D )

    int x0, y0, x, y, z;
    UINT8 tempValues[SIZE][SIZE];
    memset( tempValues, 0, SIZE * SIZE * SIZEOF(UINT8) );

    for( x = 0; x < SIZE; x++ )
    {
        for( z = 0; z < SIZE; z++ )
        {
            //x0 = ( L * SHIFT( x ) ) / ( D + z ); // bez kąta
            x0 = ( L * ( e + f ) / ( a + b ) ) * SIZE / SCREEN + SIZE_2;
            if( x0 < SIZE && x0 >= 0 )
            {
                for( y = 0; y < SIZE; y++ )
                {
                    //y0 = ( L * SHIFT( y ) ) / ( D + z ); // bez kąta
                    #define result ( ( SIZE_2 - static_cast<double>(z) ) * COS( beta ) + ( SHIFT( x ) ) * SIN( beta ) + SIZE_2 )

                    y0 = ( ( SHIFT( y ) ) * static_cast<double>(L) / ( L - result ) ) * SIZE / SCREEN + SIZE_2;
                    if( y0 < SIZE && y0 >= 0 )
                    {
                        if( TABLE2D( bitmap, x0, y0, SIZE ) == 0 )
                        {
                            tempValues[y][z] = 1;
                            //TABLE3D( cube, x, y, z, SIZE, SIZE ) += 1;
                        }
                        else // point does not belong to object
                        {
                            //DEBUG("fail")
                            //TABLE3D( cube, x, y, z, SIZE, SIZE ) = 0;
                            //DEBUG_INT( VALUE16( TABLE2D( processedBitmap16->m_bitmap, x0, y0, SIZE ) ) )
                        }
                    }
                    else // "invalid" point computation
                    {
                        //DEBUG( std::string( std::to_string( x0 ) + " : " + std::to_string( y0 ) ).c_str() );
                    }
                } // for
            } // if
        } // for
        
        g_cubeMutex[ NUM_THREADS * x / SIZE ].lock();
        for( y = 0; y < SIZE; y++ )
        {
            for( z = 0; z < SIZE; z++)
            {
                TABLE3D( cube, x, y, z, SIZE, SIZE ) += tempValues[y][z];
                tempValues[y][z] = 0;
            }
        }
        g_cubeMutex[ NUM_THREADS * x / SIZE ].unlock();
    } // for

    return COMMON::STATUS::OK;
}

/******************************************************************************/

void CORE::Computation::Destroy16()
{
    m_initialized16 = false;
    m_cube16 = nullptr;
}

/******************************************************************************/
