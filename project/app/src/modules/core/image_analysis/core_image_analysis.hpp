#ifndef CORE_IMAGE_ANALYSIS_HPP
#define CORE_IMAGE_ANALYSIS_HPP

#include "common.hpp"
#include "core.hpp"


#define RGB16(r,g,b)	( (UINT16)( ( ( ( r * 31 ) / 255 ) << 11 ) + \
						 			( ( ( g * 63 ) / 255 ) << 5 ) + \
						    		( ( ( b * 31 ) / 255 ) ) ) )

#define RED16(x)		( ( ( ( x & ( 31 << 11 ) ) >> 11 ) * 255 ) / 31 )
#define GREEN16(x)		( ( ( ( x & ( 63 <<  5 ) ) >>  5 ) * 255 ) / 63 )
#define BLUE16(x)		  ( ( ( x &   31 )                 * 255 ) / 31 )

#define VALUE16(x)		MAX3( RED16( x ), GREEN16( x ), BLUE16( x ) )


namespace CORE
{
	class ImageAnalysis
	{
		public:
			ImageAnalysis();
			~ImageAnalysis();

			bool provideBitmap16( UINT16 * bitmap, size_t sizeX, size_t sizeY );
			ProcessedBitmap16 * processBitmap16();
			void releaseBitmap16();
			bool isBitmap16Valid();

		private:
			UINT16 * m_bitmap16;
			size_t m_sizeX16;
			size_t m_sizeY16;

	};
}

#endif // CORE_IMAGE_ANALYSIS_HPP
