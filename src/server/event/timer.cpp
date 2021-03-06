#include <common/StdAfx.h>

#include <event/timer.h>


namespace smarty
{

//--------------------------------------------------------------------------------------------------

static const uint SLEEP_TIMEOUT = 1000;

//--------------------------------------------------------------------------------------------------

timer_t::timer_t( )
    : m_event_queue( )
{
}

//--------------------------------------------------------------------------------------------------

void timer_t::do_run( )
{
    while ( !is_stopping( ) )
    {
        check_event_expiration( );
        LOG_TRACE( "TIMER ." );
        sleep( SLEEP_TIMEOUT );
    }
}

//--------------------------------------------------------------------------------------------------

void timer_t::do_stop( )
{
}

//--------------------------------------------------------------------------------------------------

void timer_t::add_event( time_event_ptr_t event )
{
    m_event_queue.push( event );
}

//--------------------------------------------------------------------------------------------------

void timer_t::check_event_expiration( )
{
    while ( !m_event_queue.empty( ) )
    {
        auto event = m_event_queue.pop( );
    }
}

//--------------------------------------------------------------------------------------------------

} // namesapce smarty
