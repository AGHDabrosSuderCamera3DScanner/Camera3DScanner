#include "core_image_analysis.hpp"

using namespace COMMON;

/******************************************************************************/

CORE::ImageAnalysis::ImageAnalysis()
{
}

/******************************************************************************/

CORE::ImageAnalysis::~ImageAnalysis()
{
}

/******************************************************************************/

PBitmap16 CORE::ImageAnalysis::processBitmap16( PBitmap16& pBitmap )
{
	if( !pBitmap || !pBitmap->isValid() )
	{
		return PBitmap16( nullptr );
	}

	BITMAP16_INNER_TYPE * rawBitmap16 = pBitmap->m_bitmap.get();
    size_t sizeX16 = pBitmap->m_sizeX;
	size_t sizeY16 = pBitmap->m_sizeY;
    BITMAP16_INNER_TYPE * bitmap16 = new BITMAP16_INNER_TYPE[ sizeX16 * sizeY16 * SIZEOF(BITMAP16_INNER_TYPE) ];

	for( size_t i = 0; i < sizeX16; i++ )
	{
		for( size_t j = 0; j < sizeY16; j++ )
		{
			if( VALUE16( TABLE2D( rawBitmap16, i, j, sizeX16 ) ) >= 128 )
			{
				//TABLE2D( bitmap16, i, j, sizeX16 ) = RGB16( 0, 0, 0 );
				TABLE2D( bitmap16, i, j, sizeX16 ) = 0x0000;
			}
			else
			{
				//TABLE2D( bitmap16, i, j, sizeX16 ) = RGB16( 255, 255, 255 );
				TABLE2D( bitmap16, i, j, sizeX16 ) = 0xFFFF;
			}
		}
	}

	return PBitmap16( new Bitmap16( PInnerBitmap16( bitmap16 ), sizeX16, sizeY16 ) );
}

/******************************************************************************/
