#include <common/StdAfx.h>

#include <command/command_device.h>
#include <command/command_processor.h>
#include <device/device_controller.h>
#include <device/light_object.h>
#include <event/event_handler.h>
#include <event/event_parser.h>
#include <mobile/mobile_handler.h>
#include <mobile/mobile_register.h>
#include <server/smarty_server.h>

#include <common/smarty_config.h>
#include <common/client_protocol.h>
#include <common/config_options.h>
#include <common/constants.h>

#include <logger.h>
#include <strutils.h>


extern const char g_szUnableToSendResponse[];


// mobile_handler_t implementation
//--------------------------------------------------------------------------------------------------

mobile_handler_t::mobile_handler_t( socket_t& socket, const std::string& endpoint,
                                    smarty_config_t& config, device_controller_t& device,
                                    mobile_register_t& mobile_register,
                                    smarty_server_t& smarty_server, packet_intf_t *hs_req,
                                    command_processor_t& command_handler,
                                    event_handler_t& event_handler )
    : m_socket( socket )
    , m_end_point( endpoint )
    , m_hs_req( NULL )
    , m_config( config )
    , m_device( device )
    , m_mobile_register( mobile_register )
    , m_smarty_server( smarty_server )
    , m_command_handler( command_handler )
    , m_event_handler( event_handler )
{
    mobile_handshake_request_t *request = dynamic_cast<mobile_handshake_request_t *>( hs_req );
    ASSERT( request != NULL );

    m_hs_req = new mobile_handshake_request_t( *request );
}

//--------------------------------------------------------------------------------------------------

mobile_handler_t::~mobile_handler_t( )
{
    FREE_POINTER( m_hs_req );
}

//--------------------------------------------------------------------------------------------------

/*virtual */
bool mobile_handler_t::make_handshake( )
{
    LOG_TRACE( "[mobile] handshake request from %s", m_end_point.c_str( ) );

    mobile_handshake_response_t response;

    auto password_node = m_config[ SERVER_SECTION ][ SERVER_PASSWORD ];
    if ( !password_node.isString( ) )
    {
        LOG_ERROR( "Server password is not set" );
        return false;
    }

    std::string password = password_node.asString( );
    if ( password.empty( ) )
    {
        LOG_ERROR( "Server password cannot be empty" );
        return false;
    }

    if ( password != m_hs_req->password )
    {
        response.state = ES_INVALID_PASSWORD;
        LOG_ERROR( "[mobile] Incorrect password %s", m_hs_req->password.c_str( ) );
    }
    else
    {
        response.state = ES_OK;
        response.light_state = m_device.get_lights_state( );
        response.event_modes = m_smarty_server.get_event_handler( ).get_modes_bitset( );

        int nHeartBeatPeriod = DEFAULT_HEARTBEAT_PERIOD;
        auto hb_period_node = m_config[ SERVER_SECTION ][ SERVER_HB_PERIOD ];
        if ( hb_period_node.isInt( ) )
        {
            nHeartBeatPeriod = hb_period_node.asInt( );
            ADJUST_HEARTBEAT_PERIOD( nHeartBeatPeriod );
        }

        LOG_TRACE( "Heartbeat period set to %u", nHeartBeatPeriod );
        response.heartbeat_timeout = nHeartBeatPeriod;

        LOG_INFO( "[mobile] client %s connected", m_end_point.c_str( ) );
    }

    if ( !response.send( m_socket ) )
    {
        string error;
        StrUtils::FormatString( error, g_szUnableToSendResponse,
                                "[mobile] client handshake", m_end_point.c_str( ) );
        LOG_ERROR( error.c_str( ) );
        return false;
    }

    return response.state == ES_OK;
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void mobile_handler_t::process_client( )
{
    if ( !update_mobile_config( ) )
    {
        return;
    }

    m_mobile_register.add_client( &m_socket, m_end_point );

    auto disable_hb_node = m_config[ LOGGER ][ LOGGER_DISABLE_HB ];
    bool disable_hb_msgs = disable_hb_node.isInt( ) && disable_hb_node.asInt( ) != 0;

    while ( true )
    {
        RECV_PACKET_SWITCH( &m_socket );
        RECV_PACKET_CASE( command_request_t, request )
        {
            LOG_DEBUG( "[mobile] Command request from %s, cmd: %s (%u), param:  %2.2X",
                       m_end_point.c_str( ), get_mc_command_name( (ECOMMAND)request.command ),
                       request.command, request.param );
            m_device.execute_command( request.command, request.param );
        }
        RECV_PACKET_CASE( config_update_request_t, request )
        {
            LOG_ERROR( "[mobile] Config update request from %s", m_end_point.c_str( ) );

            if ( !send_config_update_response( ) )
            {
                LOG_DEBUG( "[mobile] disconnected  %s", m_end_point.c_str( ) );
                m_mobile_register.remove_client( &m_socket );
                return;
            }
        }
        RECV_PACKET_CASE( desktop_command_request_t, request )
        {
            LOG_DEBUG( "[mobile] Desktop command request to %u, cmd: %s (%u), params: %s",
                       request.desktop_index, get_desktop_command_name( request.command ),
                       request.command, request.params.c_str( ) );
            m_smarty_server.on_execute_desktop_command( request );
        }
        RECV_PACKET_CASE( server_command_request_t, request )
        {
            LOG_DEBUG( "[mobile] Server command request, cmd: %s (%u), params: %s",
                       get_server_command_name( request.command ), request.command,
                       request.params.c_str( ) );

            add_server_command( request );
        }
        RECV_PACKET_CASE( mobile_heartbeat_response_t, response )
        {
            if ( !disable_hb_msgs )
            {
                LOG_TRACE( "[mobile] heartbeat response from %s", m_end_point.c_str( ) );
            }
            m_mobile_register.set_heartbeat_lasttime( &m_socket );
        }
        RECV_PACKET_CASE( update_modes_request_t, request )
        {
            LOG_DEBUG( "[mobile] Update modes request from %s. State: %u",
                        m_end_point.c_str( ), request.state );
            m_smarty_server.on_update_modes_request( request );
        }
        RECV_PACKET_CASE( disconnect_request_t, request )
        {
            LOG_DEBUG( "[mobile] disconnected  %s", m_end_point.c_str( ) );
            m_mobile_register.remove_client( &m_socket );
            return;
        }
        RECV_PACKET_ERROR( message )
        {
            LOG_INFO( "[mobile] disconnected %s (%s)", m_end_point.c_str( ), message );
            m_mobile_register.remove_client( &m_socket );
            return;
        }
    }
}

//--------------------------------------------------------------------------------------------------

bool mobile_handler_t::update_mobile_config( )
{
    config_update_request_t config_request;
    if ( !config_request.recv( m_socket ) )
    {
        LOG_ERROR( "[mobile] Unable to recv update config request from %s", m_end_point.c_str( ) );
        return false;
    }

    LOG_TRACE( "[mobile] Update config request from %s", m_end_point.c_str( ) );
    return send_config_update_response( );
}

//--------------------------------------------------------------------------------------------------

bool mobile_handler_t::send_config_update_response( )
{
    if ( !m_mobile_register.update_config( m_socket ) )
    {
        string error;
        StrUtils::FormatString( error,
                                g_szUnableToSendResponse, "update config", m_end_point.c_str( ) );
        LOG_ERROR( error.c_str( ) );
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

void mobile_handler_t::add_server_command( const server_command_request_t& request )
{
    switch ( request.command )
    {
    case SERVERCOMMAND::DELAYED:
        add_command_delay( request.params );
        break;
    case SERVERCOMMAND::LIGHT:
        add_command_light( request.params );
        break;
    default:
        LOG_ERROR( "[mobile] Unknown server command: %u", (uint)request.command );
    }
}

//--------------------------------------------------------------------------------------------------

void mobile_handler_t::add_command_delay( const std::string& params )
{
    vector< string > options;
    auto count = StrUtils::split( params.c_str( ), options, ":" );
    ASSERT( count == 2 );

    uint timeout = static_cast< uint >( atoi( options[ 1 ].c_str( ) ) );
    device_param_t param = static_cast< device_param_t >( atoi( options[ 0 ].c_str( ) ) );

    auto command = m_command_handler.create_device_command( { EC_TURNOFF, param }, timeout );
    m_command_handler.add_command( command );
}

//--------------------------------------------------------------------------------------------------

void mobile_handler_t::add_command_light( const std::string& params )
{
    ASSERT_NOT_IMPLEMENTED( );
}

//--------------------------------------------------------------------------------------------------
