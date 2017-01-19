#ifndef COMMON_HPP
#define COMMON_HPP

#include <cstring>
#include <string>
#include <memory>
#include "defines.hpp"
#include <mutex>

extern LONGLONG     g_size;
extern LONGLONG     g_distance;
extern double       g_cameraAngle;
extern UINT32       g_numberOfThreads;
extern BYTE         g_minBackgroundValue;
extern std::mutex * g_pCubeMutexes;

namespace COMMON
{
    /******** types ********/

    class Bitmap16;
    class Cube16;

    typedef void( *callback_t )( void * );

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


    class Bitmap16
    {
    public:
        Bitmap16( PInnerBitmap16 );
        ~Bitmap16();
        bool isValid();

    public:
        const PInnerBitmap16 m_bitmap;
    };


    class Cube16
    {
    public:
        Cube16( PInnerCube16 cube );
        ~Cube16();
        bool isValid();

    public:
        const PInnerCube16 m_cube;
    };


    /******** functions ********/

    void exit( int error_code );

    std::string print_status( STATUS error );

    void raise_error( const char * str );
    void raise_error( const std::string str );

    void raise_perror( const char * str );
    void raise_perror( const std::string str );

    void printTable2D( BITMAP16_INNER_TYPE * table, size_t size = SIZE );
    void printTable3D( CUBE16_INNER_TYPE * table, size_t size = SIZE );
}

#endif // COMMON_HPP
