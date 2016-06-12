#include "core_image_analysis.hpp"

/******************************************************************************/

CORE::ImageAnalysis::ImageAnalysis()
{
	m_bitmap16 = NULL;
	m_sizeX16 = 0;
	m_sizeY16 = 0;
}

/******************************************************************************/

CORE::ImageAnalysis::~ImageAnalysis()
{
	releaseBitmap16();
}

/******************************************************************************/

bool CORE::ImageAnalysis::provideBitmap16( UINT16 * bitmap, size_t sizeX, size_t sizeY )
{
	if( bitmap != NULL && sizeX > 0 && sizeY > 0 )
	{
		releaseBitmap16();  // release previous bitmap to prevent memory leaks

		m_bitmap16 = bitmap;
		m_sizeX16 = sizeX;
		m_sizeY16 = sizeY;

		return true;
	}

	return false;
}

/******************************************************************************/

CORE::ProcessedBitmap16 * CORE::ImageAnalysis::processBitmap16()
{
	if( !isBitmap16Valid() )
	{
		return NULL;
	}

	CORE::ProcessedBitmap16 * processedBitmap16 = new CORE::ProcessedBitmap16(
		new UINT16[m_sizeX16 * m_sizeY16],
		m_sizeX16,
		m_sizeY16 );

	for( size_t i = 0; i < m_sizeX16; i++ )
	{
		for( size_t j = 0; j < m_sizeY16; j++ )
		{
			if( VALUE16( TABLE2D( m_bitmap16, i, j, m_sizeX16 ) ) < 220 )
			{
				TABLE2D( processedBitmap16->m_bitmap, i, j, m_sizeX16 ) = RGB16( 0, 0, 0 );
			}
			else
			{
				TABLE2D( processedBitmap16->m_bitmap, i, j, m_sizeX16 ) = RGB16( 255, 255, 255 );
			}
		}
	}

	return processedBitmap16;
}

/******************************************************************************/

void CORE::ImageAnalysis::releaseBitmap16()
{
	if( m_bitmap16 != NULL )
	{
		//delete [] m_bitmap16;
		m_bitmap16 = NULL;
	}
	m_sizeX16 = 0;
	m_sizeY16 = 0;

	return;
}

/******************************************************************************/

bool CORE::ImageAnalysis::isBitmap16Valid()
{
	if( m_bitmap16 != NULL && m_sizeX16 > 0 && m_sizeY16 > 0 )
	{
		return true;
	}

	return false;
}

/******************************************************************************/
