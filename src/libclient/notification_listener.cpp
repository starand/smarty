#include <common/StdAfx.h>
#include "notification_listener.h"

#include "smarty_connector.h"

#include <common/client_protocol.h>

#include <xsocket.h>


// notification_listener_t implementation
//--------------------------------------------------------------------------------------------------

notification_listener_t::notification_listener_t( smarty_connector_intf_t& connector,
                                                  socket_t& socket,
                                                  change_state_callback_f change_state_callback,
                                                  notification_callback_f notification_callback )
    : m_connector( connector )
    , m_socket( socket )
    , m_change_state_callback( change_state_callback )
    , m_notification_callback( notification_callback )
    , m_prev_state( 0 )
{
}

//--------------------------------------------------------------------------------------------------

notification_listener_t::~notification_listener_t( )
{
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void notification_listener_t::do_run( )
{
    while ( true )
    {
        m_connector.wait_connected( );
        if ( is_stopping( ) )
        {
            break;
        }

        ASSERT( m_socket.is_connected( ) );

        process_notification( );
    }
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void notification_listener_t::do_stop( )
{
}

//--------------------------------------------------------------------------------------------------

void notification_listener_t::process_notification( )
{
    socket_t *current_client =& m_socket;

    RECV_PACKET_SWITCH( current_client );
    RECV_PACKET_CASE( light_notification_t, notification )
    {
        if ( notification.lights_state != m_prev_state )
        {
            LOG_DEBUG( "Notification retrieved with state %u", notification.lights_state );
            m_change_state_callback( notification.lights_state );

            m_prev_state = notification.lights_state;
        }
    }
    RECV_PACKET_CASE( mobile_notification_t, notification )
    {
        LOG_DEBUG( "Mobile notification from desktop %u, type: %u, params: %s",
            notification.desktop_index, notification.type, notification.params.c_str( ) );
        if ( m_notification_callback )
        {
            m_notification_callback( &notification );
        }
    }
    RECV_PACKET_CASE( mobile_heartbeat_request_t, request )
    {
        LOG_TRACE( "mobile heart beat request retrieved" );
        mobile_heartbeat_response_t response;
        response.send( m_socket );
    }
    RECV_PACKET_CASE( disconnect_request_t, request )
    {
        LOG_WARNING( "Disconnect request from server. Reconnection .." );
        m_connector.reconnect( );
    }
    RECV_PACKET_ERROR( message )
    {
        LOG_ERROR( "Connection error (%s). Reconnection ..", message );
        m_connector.reconnect( );
    }
}

//--------------------------------------------------------------------------------------------------
