#include <common/StdAfx.h>
#include "packet_sender.h"

#include <common/client_protocol.h>

#include <scoped_ptr.h>


// packet_sender_t implementation
//--------------------------------------------------------------------------------------------------

packet_sender_t::packet_sender_t( )
    : m_send_queue( )
    , m_socket( NULL )
{
}

//--------------------------------------------------------------------------------------------------

packet_sender_t::~packet_sender_t( )
{
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void packet_sender_t::do_run( )
{
    while ( true )
    {
        scoped_ptr_t<packet_intf_t> packet( m_send_queue.pop( ) );
        if ( is_stopping( ) )
        {
            break;
        }

        if ( m_socket )
        {
            if ( !packet->send( *m_socket ) )
            {
                LOG_ERROR( "Could not send packet to server" );
                m_socket = NULL;
            }
        }
    }
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void packet_sender_t::do_stop( )
{
    packet_intf_t *packet = new desktop_heartbeat_response_t( );
    m_send_queue.push( packet );
}

//--------------------------------------------------------------------------------------------------

void packet_sender_t::set_socket( socket_t *socket )
{
    m_socket = socket;
}

//--------------------------------------------------------------------------------------------------

socket_t *packet_sender_t::get_socket( ) const
{
    return m_socket;
}

//--------------------------------------------------------------------------------------------------

void packet_sender_t::send_packet( packet_intf_t *packet )
{
    m_send_queue.push( packet );
}

//--------------------------------------------------------------------------------------------------
