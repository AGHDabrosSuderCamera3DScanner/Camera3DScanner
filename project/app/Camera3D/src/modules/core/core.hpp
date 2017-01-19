#ifndef CORE_HPP
#define CORE_HPP

#include "common.hpp"

#include "core_image_analysis.hpp"
#include "core_computation.hpp"

namespace CORE
{
    class Core
    {
    public:
        Core();
        ~Core();
        COMMON::STATUS Init16( COMMON::PCube16& pCube16 );
        COMMON::STATUS ProvideBitmap16( COMMON::PBitmap16& pBitmap16, double angle );
        COMMON::STATUS Start16( COMMON::callback_t callback );
        void ReleaseBitmap16();
        void Destroy16();

    private:
        CORE::ImageAnalysis     m_imageAnalysis;
        CORE::Computation       m_computation;
        COMMON::PCube16         m_cube16;
        COMMON::PBitmap16       m_bitmap16;
        bool                    m_initialized16;
        bool                    m_bitmap16Provided;
        double                  m_angle16;

    };
}


#endif // CORE_HPP
