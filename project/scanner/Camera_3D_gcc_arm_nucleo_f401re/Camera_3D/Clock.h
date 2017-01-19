#include "mbed.h"

#define SEC(x)     ( ( x ) * 1000000 )  // us
#define MSEC(x)    ( ( x ) * 1000 )     // us
#define USEC(x)    ( x )                // us

class Clock
{
public:
    Clock( PinName CLK );
    ~Clock();

    void Interval( unsigned int sec );
    void Interval_ms( unsigned int msec );
    void Interval_us( unsigned int usec );

    void Set( bool value );
    bool Get();
    void Switch();
    void Cycle();
    
    Clock& operator =( bool value );
    operator bool();

private:
    DigitalOut  m_Pin;
    int         m_Interval_us;
    
};