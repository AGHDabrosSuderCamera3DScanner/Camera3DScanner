#ifndef COMMON_HPP
#define COMMON_HPP

#include "defines.hpp"
#include <memory>
//#include <string>

namespace COMMON
{
    class Bitmap16;
    class Cube16;

    typedef void(*callback_t)( void * );

    typedef std::shared_ptr<Bitmap16>               PBitmap16;
    typedef std::shared_ptr<Cube16>                 PCube16;
    typedef std::shared_ptr<BITMAP16_INNER_TYPE>    PInnerBitmap16;
    typedef std::shared_ptr<CUBE16_INNER_TYPE>      PInnerCube16;

    enum class STATUS
    {
        OK = 0,
        ERROR,
        INVALID_BITMAP,
        INVALID_CUBE,
        NOT_INITIALIZED,
        ALREADY_INITIALIZED,
        BITMAP_NOT_PROVIDED,
        BITMAP_ALREADY_PROVIDED,
    }; // when updating this enum, update also 'print_status(...)' function

    std::string print_status( STATUS error );

	void raise_error( const char * str );
	void raise_error( const std::string str );

	void raise_perror( const char * str );
	void raise_perror( const std::string str );

	class Bitmap16
	{
		public:
			Bitmap16(
                PInnerBitmap16 bitmap,
                size_t sizeX,
                size_t sizeY );
			~Bitmap16();
			bool isValid();

		public:
			const PInnerBitmap16    m_bitmap;
			const size_t            m_sizeX;
			const size_t            m_sizeY;
	};

	class Cube16
	{
		public:
			Cube16(
                PInnerCube16 cube,
                size_t sizeX,
                size_t sizeY,
                size_t sizeZ );
			~Cube16();
			bool isValid();

		public:
			const PInnerCube16      m_cube;
			const size_t            m_sizeX;
			const size_t            m_sizeY;
			const size_t            m_sizeZ;
	};
}

#endif // COMMON_HPP
