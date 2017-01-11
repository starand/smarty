#include <common/StdAfx.h>
#include "smarty_connector.h"

#include "notification_listener.h"
#include "command_sender.h"

#include <common/enums.h>

#include <strutils.h>
#include <xsocket.h>
#include <utils.h>
#include <fileutils.h>


#define RECONNECT_TIMEOUT	5000


static const string g_sServerConfigFileName = "server.cfg";

// smarty_connector_t implementation
//--------------------------------------------------------------------------------------------------

smarty_connector_t::smarty_connector_t( change_state_callback_f change_state_callback,
                                        notification_callback_f notification_callback )
    : m_socket( NULL )
    , m_change_state_callback( change_state_callback )
    , m_notification_callback( notification_callback )
    , m_send_queue( )
    , m_command_sender( NULL )
    , m_notification_listener( NULL )
    , m_connection_start_event( false, false )
    , m_connection_finished_event( true, false )
    , m_connection_established( true, false )
    , m_connected( false )
    , m_host( )
    , m_port( 0 )
    , m_password( )
    , m_last_error( )
    , m_config( )
{
    create_internal_objects( );

    ASSERT( thread_base_t::start( ) );
}

//--------------------------------------------------------------------------------------------------

smarty_connector_t::~smarty_connector_t( )
{
    thread_base_t::stop( );
    thread_base_t::wait( );

    destroy_internal_objects( );
}

//--------------------------------------------------------------------------------------------------

void smarty_connector_t::create_internal_objects( )
{
    ASSERT( m_socket == NULL );
    ASSERT( m_notification_listener == NULL && m_command_sender == NULL );

    m_socket = new socket_t( );

    m_notification_listener =
        new notification_listener_t( *this, *m_socket, m_change_state_callback,
        m_notification_callback );
    m_command_sender = new command_sender_t( *this, *m_socket, m_send_queue );
}

//--------------------------------------------------------------------------------------------------

void smarty_connector_t::destroy_internal_objects( )
{
    ASSERT( m_socket != NULL && m_notification_listener != NULL && m_command_sender != NULL );

    m_command_sender->stop( );
    m_notification_listener->stop( );

    m_command_sender->wait( );
    m_notification_listener->wait( );

    delete m_command_sender;
    delete m_notification_listener;
    delete m_socket;

    // TODO : cleanup queue -- inter in contains pointers
    m_send_queue.cleanup( );
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void smarty_connector_t::do_run( )
{
    bool first_connect = true;

    // start worker threads
    ASSERT( m_notification_listener->start( ) );
    ASSERT( m_command_sender->start( ) );

    while ( true )
    {
        m_connection_start_event.wait( );
        if ( is_stopping( ) )
        {
            break;
        }

        m_connected = do_connect( ) && make_handshake( ) && update_config( );
        m_connection_finished_event.set( );

        if ( m_connected )
        {
            reconnected_notification( );

            m_connection_established.set( );
            LOG_DEBUG( "Connection established" );
        }
        else
        {
            if ( first_connect )
            {
                break;
            }
            else
            {
                utils::sleep_ms( RECONNECT_TIMEOUT );
                m_connection_start_event.set( );

                connection_finalize( );
            }
        }

        first_connect = false;
    }

    connection_finalize( );
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void smarty_connector_t::do_stop( )
{
    connection_finalize( ); // workers jump at the beginning of loop

    m_connection_start_event.set( ); // to stop connector thread
    m_connection_finished_event.set( ); // to stop worker threads
}

//--------------------------------------------------------------------------------------------------

bool smarty_connector_t::connect( const char *host, ushort port, const char *password )
{
    ASSERT( host != NULL ); ASSERT( password != NULL ); ASSERT( port > 0 );
    m_host = host; m_port = port; m_password = password;

    m_connection_start_event.set( );
    m_connection_finished_event.wait( );

    return m_connected;
}

//--------------------------------------------------------------------------------------------------

void smarty_connector_t::disconnect( )
{
    ASSERT( m_socket != NULL );

    if ( m_socket->is_connected( ) )
    {
        disconnect_request_t disconnect_request;
        if ( !disconnect_request.send( *m_socket ) )
        {
            StrUtils::FormatString( m_last_error, "Could not send disconnect request to %s:%u",
                m_host.c_str( ), m_port );
            LOG_ERROR( m_last_error.c_str( ) );
        }
    }

    connection_finalize( );
}

//--------------------------------------------------------------------------------------------------

void smarty_connector_t::reconnect( )
{
    connection_finalize( );
    m_connection_start_event.set( );
}

//--------------------------------------------------------------------------------------------------

void smarty_connector_t::wait_connected( )
{
    m_connection_established.wait( );
}

//--------------------------------------------------------------------------------------------------

void smarty_connector_t::turn_light( uchar command, uchar param )
{
    command_request_t *request = new command_request_t( );

    request->command = command;
    request->param = param;

    packet_intf_t *packet = request;
    m_send_queue.push( packet );
}

//--------------------------------------------------------------------------------------------------

void smarty_connector_t::desktop_command( const desktop_command_request_t& command )
{
    packet_intf_t *packet = new desktop_command_request_t( command );
    m_send_queue.push( packet );
}

//--------------------------------------------------------------------------------------------------

ErrorCode smarty_connector_t::server_command( const server_command_request_t& cmd )
{
    packet_intf_t* packet = new server_command_request_t( cmd );
    m_send_queue.push( packet );

    return ErrorCode::OK;
}

//--------------------------------------------------------------------------------------------------

const char *smarty_connector_t::get_last_error( ) const
{
    return m_last_error.c_str( );
}

//--------------------------------------------------------------------------------------------------

const char *smarty_connector_t::get_config( ) const
{
    return m_config.c_str( );
}

//--------------------------------------------------------------------------------------------------

bool smarty_connector_t::do_connect( )
{
    ASSERT( m_socket != NULL ); ASSERT( !m_socket->is_connected( ) );

    if ( !m_socket->connect( m_host.c_str( ), m_port ) )
    {
        //StrUtils::FormatString(m_last_error, "Could not connect to %s:%u", m_host.c_str(), m_port);
        m_last_error = "Could not connect to server.";

        LOG_ERROR( m_last_error.c_str( ) );
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

bool smarty_connector_t::make_handshake( )
{
    ASSERT( m_socket != NULL ); ASSERT( m_socket->is_connected( ) );

    mobile_handshake_request_t request;
    request.password = m_password;
    request.version = SMARTY_VERSION;

    if ( !request.send( *m_socket ) )
    {
        StrUtils::FormatString( m_last_error, "Could not send handshake request to %s:%u",
            m_host.c_str( ), m_port );
        LOG_ERROR( m_last_error.c_str( ) );
        return false;
    }

    mobile_handshake_response_t response;
    if ( !response.recv( *m_socket ) )
    {
        StrUtils::FormatString( m_last_error, "Could not receive handshake response from %s:%u",
            m_host.c_str( ), m_port );
        LOG_ERROR( m_last_error.c_str( ) );
        return false;
    }

    if ( !check_handshake_response( response ) )
    {
        return false;
    }

    m_change_state_callback( response.light_state );

    return true;
}

//--------------------------------------------------------------------------------------------------

bool smarty_connector_t::check_handshake_response( mobile_handshake_response_t& response )
{
    switch ( response.state )
    {
    case ES_OK:
        return true;
    case ES_INVALID_PASSWORD:
        m_last_error = "Incorrect password";
        LOG_ERROR( m_last_error.c_str( ) );
        return false;
    case ES_ERROR:
        m_last_error = "Server error";
        LOG_ERROR( m_last_error.c_str( ) );
        return false;
    default:
        bool invalid_handshake_response_state = false;
        ASSERT( invalid_handshake_response_state );
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

void smarty_connector_t::connection_finalize( )
{
    ASSERT( m_socket != NULL );
    m_connection_established.reset( );

    if ( m_socket->is_connected( ) )
    {
        m_socket->shutdown( );
        m_socket->close( );
    }

    // TODO : correct workers
}

//--------------------------------------------------------------------------------------------------

bool smarty_connector_t::update_config( )
{
    ASSERT( m_socket != NULL ); ASSERT( m_socket->is_connected( ) );

    config_update_request_t request;
    if ( !request.send( *m_socket ) )
    {
        StrUtils::FormatString( m_last_error, "Could not send config request to %s:%u",
            m_host.c_str( ), m_port );
        LOG_ERROR( m_last_error.c_str( ) );
        return false;
    }

    config_update_response_t config;
    if ( !config.recv( *m_socket ) )
    {
        StrUtils::FormatString( m_last_error, "Could not receive update config response from %s:%u",
            m_host.c_str( ), m_port );
        LOG_ERROR( m_last_error.c_str( ) );
        return false;
    }

    string config_file = g_sServerConfigFileName;

#ifndef __ANDROID__
    string binary_folder;
    if ( !FileUtils::GetBinaryDir( binary_folder ) )
    {
        m_last_error = "Unable to retrieve binary folder";
        LOG_ERROR( m_last_error.c_str( ) );
        return false;
    }

    config_file = binary_folder + config_file;
#endif

    if ( !FileUtils::PutFileContent( config_file, config.config ) )
    {
        StrUtils::FormatString( m_last_error, "Could not save config to file",
            config_file.c_str( ) );
        LOG_ERROR( m_last_error.c_str( ) );
        return false;
    }

    m_config = config.config;
    return true;
}

//--------------------------------------------------------------------------------------------------

void smarty_connector_t::reconnected_notification( )
{
    ASSERT( m_notification_callback != NULL );

    mobile_notification_t notification;
    notification.desktop_index = 0;
    notification.type = EMN_RECONNECTED;

    m_notification_callback( &notification );
}

//--------------------------------------------------------------------------------------------------
