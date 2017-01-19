#include "Clock.h"

Clock::Clock( PinName CLK ) : m_Pin( CLK ), m_Interval_us( USEC( 1000 ) )
{
    Set( 1 );
}

Clock::~Clock()
{
}

void Clock::Interval( unsigned int sec )
{
    m_Interval_us = SEC( sec );
}

void Clock::Interval_ms( unsigned int msec )
{
    m_Interval_us = MSEC( msec );
}

void Clock::Interval_us( unsigned int usec )
{
    m_Interval_us = USEC( usec );
}

void Clock::Set( bool value )
{
    m_Pin = static_cast<int>( value );
    wait_us( m_Interval_us );
}

bool Clock::Get()
{
    return m_Pin != 0;
}

void Clock::Switch()
{
    m_Pin = !m_Pin;
    wait_us( m_Interval_us );
}

void Clock::Cycle()
{
    Switch();
    Switch();
}

Clock& Clock::operator =( bool value )
{
    Clock::Set( value );
    return *this;
}

Clock::operator bool()
{
    return Clock::Get();
}
