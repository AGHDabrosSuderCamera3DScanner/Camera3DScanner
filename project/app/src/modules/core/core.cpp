#include "core.hpp"

/******************************************************************************/

CORE::ProcessedBitmap16::ProcessedBitmap16()
{
	m_bitmap = NULL;
	m_sizeX = 0;
	m_sizeY = 0;
}

/******************************************************************************/

CORE::ProcessedBitmap16::ProcessedBitmap16(
	UINT16 * bitmap,
	size_t sizeX,
	size_t sizeY )
{
	m_bitmap = bitmap;
	m_sizeX = sizeX;
	m_sizeY = sizeY;
}

/******************************************************************************/

CORE::ProcessedBitmap16::~ProcessedBitmap16()
{
	release();
}

/******************************************************************************/

void CORE::ProcessedBitmap16::release()
{
	if( m_bitmap != NULL )
	{
		delete [] m_bitmap;
		m_bitmap = NULL;
	}
	m_sizeX = 0;
	m_sizeY = 0;
	
	return;
}

/******************************************************************************/

bool CORE::ProcessedBitmap16::isValid()
{
	if( m_bitmap != NULL && m_sizeX > 0 && m_sizeY > 0 )
	{
		return true;
	}
	
	return false;
}

/******************************************************************************/
