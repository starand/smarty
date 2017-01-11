#include <common/StdAfx.h>

#include "command_sender.h"
#include "smarty_connector.h"

#include <scoped_ptr.h>


// command_sender_t implementation
//--------------------------------------------------------------------------------------------------

command_sender_t::command_sender_t( smarty_connector_intf_t& connector, socket_t& socket,
                                    send_queue_t& send_queue )
    : m_connector( connector )
    , m_socket( socket )
    , m_send_queue( send_queue )
{
}

//--------------------------------------------------------------------------------------------------

command_sender_t::~command_sender_t( )
{

}

//--------------------------------------------------------------------------------------------------

/*virtual */
void command_sender_t::do_run( )
{
    while ( true )
    {
        m_connector.wait_connected( );
        if ( is_stopping( ) )
        {
            break;
        }

        scoped_ptr_t<packet_intf_t> request( m_send_queue.pop( ) );
        if ( is_stopping( ) )
        {
            break;
        }

        if ( !request->send( m_socket ) )
        {
            LOG_ERROR( "Could not send command request" );
            m_connector.reconnect( );
        }
    }
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void command_sender_t::do_stop( )
{
    packet_intf_t *request = new command_request_t( );
    m_send_queue.push( request );
}

//--------------------------------------------------------------------------------------------------
