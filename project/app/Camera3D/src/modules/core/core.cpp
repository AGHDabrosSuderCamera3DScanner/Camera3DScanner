#include "core.hpp"

/******************************************************************************/

CORE::Core::Core()
    : m_cube16( nullptr )
    , m_bitmap16( nullptr )
    , m_initialized16( false )
    , m_bitmap16Provided( false )
    , m_angle16( 0.0 )
{
}

/******************************************************************************/

CORE::Core::~Core()
{
}

/******************************************************************************/

COMMON::STATUS CORE::Core::Init16( COMMON::PCube16& pCube16 )
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
    COMMON::STATUS result = m_computation.Init16( m_cube16 );
    if( result != COMMON::STATUS::OK )
    {
        return result;
    }

    m_initialized16 = true;

    return COMMON::STATUS::OK;
}

/******************************************************************************/

COMMON::STATUS CORE::Core::ProvideBitmap16( COMMON::PBitmap16& pBitmap16, double angle )
{
    if( !m_initialized16 )
    {
        return COMMON::STATUS::NOT_INITIALIZED;
    }

    if( m_bitmap16Provided )
    {
        return COMMON::STATUS::BITMAP_ALREADY_PROVIDED;
    }

    if( !pBitmap16 || !pBitmap16->isValid() )
    {
        return COMMON::STATUS::INVALID_BITMAP;
    }

    m_bitmap16 = pBitmap16;
    m_angle16 = angle;

    m_bitmap16Provided = true;

    return COMMON::STATUS::OK;
}

/******************************************************************************/

COMMON::STATUS CORE::Core::Start16( COMMON::callback_t callback )
{
    COMMON::STATUS result;

    if( !m_initialized16 )
    {
        return COMMON::STATUS::NOT_INITIALIZED;
    }

    if( !m_bitmap16Provided )
    {
        return COMMON::STATUS::BITMAP_NOT_PROVIDED;
    }

    // --- processing image ---

    COMMON::PBitmap16 processedBitmap16 = m_imageAnalysis.processBitmap16( m_bitmap16 );
    if( !processedBitmap16 )
    {
        //raise_error( "Processed bitmap16 is null" );
        return COMMON::STATUS::ERROR;
    }
    if( !processedBitmap16->isValid() )
    {
        //raise_error( "Processed bitmap16 is not valid" );
        return COMMON::STATUS::ERROR;
    }

    // --- processing mesh ---

    result = m_computation.Compute16( processedBitmap16, m_angle16 );
    if( result != COMMON::STATUS::OK )
    {
        //raise_error( "Computation->Compute16: " + print_status( result ) );
        return COMMON::STATUS::ERROR;
    }

    // --- callback ---

    if( callback != nullptr )
    {
        callback( nullptr );
    }

    return COMMON::STATUS::OK;
}

/******************************************************************************/

void CORE::Core::ReleaseBitmap16()
{
    m_bitmap16Provided = false;
    m_bitmap16 = nullptr;
}

/******************************************************************************/

void CORE::Core::Destroy16()
{
    m_initialized16 = false;
    m_computation.Destroy16();
    m_cube16 = nullptr;
}

/******************************************************************************/

