#include <common/StdAfx.h>

#include <desktop/desktop_handler.h>
#include <desktop/desktop_register.h>
#include <server/smarty_server.h>

#include <common/smarty_config.h>
#include <common/client_protocol.h>
#include <common/config_options.h>

#include <logger.h>
#include <strutils.h>
#include <type_cast.h>


extern const char g_szUnableToSendResponse[];


// desktop_handler_t implementation
//--------------------------------------------------------------------------------------------------

desktop_handler_t::desktop_handler_t( socket_t& socket, const std::string& endpoint,
                                      smarty_config_t& config, desktop_register_t& desktop_register,
                                      smarty::client_linker_t& smarty_server, packet_intf_t *hs_req )
    : m_client_name( )
    , m_end_point( endpoint )
    , m_desktop_index( 0 )
    , m_hs_req( NULL )
    , m_socket( socket )
    , m_config( config )
    , m_desktop_register( desktop_register )
    , m_smarty_server( smarty_server )
{
    desktop_handshake_request_t *request = dynamic_cast<desktop_handshake_request_t *>( hs_req );
    ASSERT( request != NULL );

    m_hs_req = new desktop_handshake_request_t( *request );
}

//--------------------------------------------------------------------------------------------------

desktop_handler_t::~desktop_handler_t( )
{
    FREE_POINTER( m_hs_req );
}

//--------------------------------------------------------------------------------------------------

/*virtual */
bool desktop_handler_t::make_handshake( )
{
    m_client_name = m_hs_req->name;
    m_desktop_index = m_hs_req->index;

    LOG_INFO( "[desktop] %s [%u] connected %s",
              m_client_name.c_str( ), m_desktop_index, m_end_point.c_str( ) );

    auto password_node = m_config[ SERVER_SECTION ][ SERVER_PASSWORD ];
    if ( !password_node.isString( ) )
    {
        LOG_ERROR( "Server password not set" );
        return false;
    }

    string password = password_node.asString( );
    if ( password.empty( ) )
    {
        LOG_ERROR( "Password cannot be empty" );
        return false;
    }

    desktop_handshake_response_t response;
    if ( password != m_hs_req->password )
    {
        response.state = ES_INVALID_PASSWORD;
        LOG_ERROR( "[desktop] Incorrect password %s", m_hs_req->password.c_str( ) );
    }
    else
    {
        response.state = ES_OK;
    }

    if ( !response.send( m_socket ) )
    {
        string error;
        StrUtils::FormatString( error, g_szUnableToSendResponse,
                                "[desktop] handshake", m_end_point.c_str( ) );
        LOG_ERROR( error.c_str( ) );
        return false;
    }

    return response.state == ES_OK;
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void desktop_handler_t::process_client( )
{
    size_t colon_pos = m_end_point.find( ':' );
    if ( colon_pos == std::string::npos )
    {
        LOG_ERROR( "[desktop] Could not locate colon pos in end_point: %s", m_end_point.c_str( ) );
        return;
    }

    m_desktop_register.add_desktop( &m_socket, m_desktop_index, m_client_name );

    auto disable_hb_node = m_config[ LOGGER ][ LOGGER_DISABLE_HB ];
    bool disable_hb_msgs = disable_hb_node.isInt( ) && disable_hb_node.asInt( ) != 0;

    while ( true )
    {
        RECV_PACKET_SWITCH( &m_socket );
        RECV_PACKET_CASE( mobile_notification_t, notification )
        {
            LOG_DEBUG( "[desktop] Notification from desktop %s (%u), type: %s (%u), params: %s",
                m_end_point.c_str( ), notification.desktop_index,
                get_mobile_notification_name( notification.type ), notification.type,
                notification.params.c_str( ) );

            m_smarty_server.on_notify_mobile_clients( notification );
        }
        RECV_PACKET_CASE( desktop_heartbeat_response_t, resposne )
        {
            if ( !disable_hb_msgs )
            {
                LOG_TRACE( "[desktop] heartbeat response from %s", m_end_point.c_str( ) );
            }

            m_desktop_register.set_heartbeat_lasttime( resposne.desktop_index );
        }
        RECV_PACKET_CASE( disconnect_request_t, request )
        {
            LOG_DEBUG( "[desktop] disconnected %s", m_end_point.c_str( ) );
            m_desktop_register.remove_desktop( &m_socket );
            return;
        }
        RECV_PACKET_ERROR( message )
        {
            LOG_INFO( "[desktop] disconnected %s (%s)", m_end_point.c_str( ), message );
            m_desktop_register.remove_desktop( &m_socket );
            return;
        }
    }
}

//--------------------------------------------------------------------------------------------------
