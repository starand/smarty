#include <common/StdAfx.h>
#include "smarty_connector.h"

#include <common/client_protocol.h>

#include <config.h>
#include <utils.h>
#include <System.h>

#include <json/value.h>


#define RETRY_TIMEOUT	5000


// smarty_connector_t implementation
//--------------------------------------------------------------------------------------------------

smarty_connector_t::smarty_connector_t( config_t& config )
    : m_config( config )
    , m_connection_start_event( false, false )
    , m_connected( false )
    , m_connection_established( false )
    , m_socket( )
    , m_hash( 0 )
{

}

//--------------------------------------------------------------------------------------------------

smarty_connector_t::~smarty_connector_t( )
{

}

//--------------------------------------------------------------------------------------------------

/*virtual */
void smarty_connector_t::do_run( )
{
    auto server_node = m_config[ "net" ][ "server" ];
    auto port_node = m_config[ "net" ][ "port" ];
    if ( server_node.isNull( ) || port_node.isNull( ) )
    {
        LOG_ERROR( "Could not retrieve server or port value" );
        return;
    }

    string server = server_node.asString( );
    ushort port = static_cast<ushort>( port_node.asUInt( ) );

    while ( true )
    {
        m_connection_start_event.wait( );

        do
        {
            if ( is_stopping( ) )
            {
                return;
            }

            if ( !m_socket.connect( server, port ) )
            {
                LOG_ERROR( "Could not connect to %s:%u", server.c_str( ), port );
                utils::sleep_ms( RETRY_TIMEOUT );
            }
            else
            {
                recalculate_hash( );
                m_connected = make_handshake( );
                break;
            }
        }
        while ( true );

        m_connection_established.set( );
    }
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void smarty_connector_t::do_stop( )
{
    m_connection_start_event.set( );
}

//--------------------------------------------------------------------------------------------------

bool smarty_connector_t::connect( )
{
    // set connect event to start connection thread and wait for connection
    m_connection_start_event.set( );
    m_connection_established.wait( );

    return m_connected;
}

//--------------------------------------------------------------------------------------------------

void smarty_connector_t::finish_connection( )
{
    m_socket.close( );
    m_connected = false;
}

//--------------------------------------------------------------------------------------------------

socket_t *smarty_connector_t::get_socket( ) const
{
    return const_cast<socket_t *>( &m_socket );
}

//--------------------------------------------------------------------------------------------------

uint smarty_connector_t::get_hash( ) const
{
    return m_hash;
}

//--------------------------------------------------------------------------------------------------

bool smarty_connector_t::make_handshake( )
{
    ASSERT( m_socket.is_connected( ) == true );

    desktop_handshake_request_t request;

    auto password_node = m_config[ "net" ][ "password" ];
    if ( password_node.isNull( ) )
    {
        LOG_WARNING( "Could not retrieve password from config." );
        return false;
    }

    string password = password_node.asString( );

    request.index = m_hash;
    request.password = password;
    System::GetCompName( request.name );

    if ( !request.send( m_socket ) )
    {
        LOG_ERROR( "Could not sent desktop handshake request" );
        return false;
    }

    desktop_handshake_response_t response;
    if ( !response.recv( m_socket ) )
    {
        LOG_ERROR( "Could not receive desktop handshake response" );
        return false;
    }

    switch ( response.state )
    {
    case ES_INVALID_PASSWORD:
        LOG_ERROR( "Incorrect password" );
        break;
    case ES_ERROR:
        LOG_ERROR( "Server error" );
        break;
    case ES_OK:
        LOG_INFO( "Connection to %s established", m_socket.get_remote_endpoint( ).c_str( ) );
    }

    return response.state == ES_OK;
}

//--------------------------------------------------------------------------------------------------

void smarty_connector_t::recalculate_hash( )
{
    std::string host_name;
    System::GetCompName( host_name );

    std::string hash_str = m_socket.get_local_address( ) + host_name;
    uint hash = 0;

    size_t len = hash_str.length( );
    for ( size_t idx = 0; idx < len; ++idx )
    {
        hash = ( hash * 17 ) ^ hash_str[ idx ];
    }

    m_hash = hash;
}

//--------------------------------------------------------------------------------------------------
