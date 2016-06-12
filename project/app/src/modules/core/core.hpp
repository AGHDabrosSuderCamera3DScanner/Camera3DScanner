#ifndef CORE_HPP
#define CORE_HPP

#include "common.hpp"

namespace CORE
{
	class ProcessedBitmap16
	{
		public:
			ProcessedBitmap16();
			ProcessedBitmap16( UINT16 * bitmap,
								size_t sizeX,
								size_t sizeY );
			~ProcessedBitmap16();
			void release();
			bool isValid();

		public:
			UINT16 * m_bitmap;
			size_t m_sizeX;
			size_t m_sizeY;
	};
}

#include "core_image_analysis.hpp"
#include "core_computation.hpp"




#endif // CORE_HPP
