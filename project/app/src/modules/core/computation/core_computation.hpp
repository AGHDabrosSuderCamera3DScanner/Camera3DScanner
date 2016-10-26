#ifndef CORE_COMPUTATION_HPP
#define CORE_COMPUTATION_HPP

#include "common.hpp"

namespace CORE
{
    class Computation
    {
        public:
            Computation();
            COMMON::STATUS Init16( COMMON::PCube16& pCube16 );
            COMMON::STATUS Compute16( COMMON::PBitmap16& pBitmap16, double angle );
            void Destroy16();
            ~Computation();

        private:
            bool                m_initialized16;
            COMMON::PCube16     m_cube16;
    };
}

#endif // CORE_COMPUTATION_HPP
