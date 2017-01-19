#ifndef CORE_IMAGE_ANALYSIS_HPP
#define CORE_IMAGE_ANALYSIS_HPP

#include "common.hpp"

namespace CORE
{
    class ImageAnalysis
    {
    public:
        ImageAnalysis();
        ~ImageAnalysis();

        COMMON::PBitmap16 processBitmap16( COMMON::PBitmap16& pBitmap );
    };
}

#endif // CORE_IMAGE_ANALYSIS_HPP
