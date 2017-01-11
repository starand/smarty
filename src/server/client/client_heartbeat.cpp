#include <common/StdAfx.h>

#include <client/client_heartbeat.h>


#define CHECK_PERIOD_DEF	5


//--------------------------------------------------------------------------------------------------

client_heartbeat_t::client_heartbeat_t( )
    : m_heartbeat_event( false, false )
    , m_heartbeat_timeout( CHECK_PERIOD_DEF )
{
}

//--------------------------------------------------------------------------------------------------

client_heartbeat_t::~client_heartbeat_t( )
{
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void client_heartbeat_t::do_run( )
{
    while ( true )
    {
        m_heartbeat_event.wait( m_heartbeat_timeout * 1000 );
        if ( is_stopping( ) )
        {
            break;
        }

        check_liveness( );
    }
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void client_heartbeat_t::do_stop( )
{
    m_heartbeat_event.set( );
}

//--------------------------------------------------------------------------------------------------

size_t client_heartbeat_t::get_timeout( ) const
{
    return m_heartbeat_timeout;
}

//--------------------------------------------------------------------------------------------------
