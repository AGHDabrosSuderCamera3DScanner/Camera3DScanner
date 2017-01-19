#include "mbed.h"
#include "ov7670reg.h"

#define OV7670_ADDR       (0x21)
#define OV7670_WRITE      (OV7670_ADDR << 1)
#define OV7670_READ       (OV7670_WRITE | 1)
#define OV7670_REGMAX     (201)
#define OV7670_I2CFREQ    (100) 
#define OV7670_WRITEWAIT  (20)
#define OV7670_NOACK      (0)

class OV7670Camera
{
    public:
        OV7670Camera(
            PinName SCL,
            PinName SDA,
            PinName VSYNC,
            PinName HREF,
            PinName D7,
            PinName D6,
            PinName D5,
            PinName D4,
            PinName D3,
            PinName D2,
            PinName D1,
            PinName D0 );
        ~OV7670Camera( void );
        
        int  Init( const char * format, int x, int y );
        void WriteReg( int addr, int data );
        int  ReadReg( int addr );
        void Reset( void );
        char ReadByte( void );
        
        void VSyncRiseHandler( void );
        void VSyncFallHandler( void );
        void HRefRiseHandler( void );
        
        void EnableHRefRiseHandler( void );
        void DisableHRefRiseHandler( void );
    
    private:
        I2C             m_I2C;
        InterruptIn     m_VSync;
        InterruptIn     m_HRef;
        BusIn           m_Data;
        short           m_RowCount;
        int             m_PixelCount;
};