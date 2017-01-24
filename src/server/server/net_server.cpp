#include <common/StdAfx.h>

#include <client/client_thread.h>
#include <desktop/desktop_handler.h>
#include <desktop/desktop_register.h>
#include <device/device.h>
#include <mobile/mobile_handler.h>
#include <mobile/mobile_register.h>
#include <server/net_server.h>
//#include <common/client_protocol.h>

#include <net/xsocket.h>


#define DEFAULT_PORT	53074
#define HANDLERS_COUNT	10


// net_server_t implementation
//--------------------------------------------------------------------------------------------------

net_server_t::net_server_t( const config_t& config, device_t& device,
                            event_handler_t& handler )
    : m_config( config )
    , m_device( device )
    , m_event_handler( handler )
    , m_socket( new socket_t( ) )
    , m_port( DEFAULT_PORT )
    , m_clients_queue( )
    , m_client_handlers( HANDLERS_COUNT )
    , m_mobile_register( )
    , m_desktop_register( new desktop_register_t( *this ) )
{
    ASSERT( m_socket.get( ) );
    ASSERT( m_desktop_register.get( ) != nullptr );
}

//--------------------------------------------------------------------------------------------------

net_server_t::~net_server_t()
{
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void net_server_t::do_run( )
{
    if ( !init( ) )
    {
        return;
    }

    LOG_INFO( "[server] Listening on port %u", m_port );

    while ( true )
    {
        socket_t *client = m_socket->accept( );
        if ( is_stopping( ) )
        {
            break;
        }

        ASSERT( client != nullptr );
        m_clients_queue.push( client );
    }

    finalize( );
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void net_server_t::do_stop( )
{
    m_socket->shutdown( SD_BOTH );
    m_socket->close( );
}

//--------------------------------------------------------------------------------------------------

bool net_server_t::init( )
{
    ASSERT( m_socket != nullptr );

    if ( !start_mobile_register( ) || !start_desktop_register( ) )
    {
        return false;
    }

    if ( !start_client_handlers( ) )
    {
        return false;
    }

    return m_socket->listen( m_port );
}

//--------------------------------------------------------------------------------------------------

void net_server_t::finalize( )
{
    m_socket->close( );

    stop_client_handlers();
    clean_clients_queue( );

    stop_desktop_register( );
    stop_mobile_register( );
}

//--------------------------------------------------------------------------------------------------

void net_server_t::clean_clients_queue( )
{
    while ( !m_clients_queue.empty( ) )
    {
        m_clients_queue.pop( );
    }
}

//--------------------------------------------------------------------------------------------------

bool net_server_t::start_client_handlers( )
{
    for ( auto& handler : m_client_handlers )
    {
        handler.reset( new client_thread_t( *this, m_clients_queue ) );
        if ( !handler->start( ) )
        {
            ASSERT_FAIL( "Unable to start client handler" );
        }
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

void net_server_t::stop_client_handlers( )
{
    for ( auto& handler : m_client_handlers )
    {
        ASSERT( handler.get( ) != nullptr );

        handler->stop( );
        handler->wait( );
    }
}

//--------------------------------------------------------------------------------------------------

bool net_server_t::start_mobile_register( )
{
    ASSERT( m_mobile_register.get( ) == nullptr );

    m_mobile_register.reset( new mobile_register_t( *this, m_config ) );
    if ( !m_mobile_register->start( ) )
    {
        ASSERT_FAIL( "Unable to start mobile register" );
    }

    m_device.add_observer( *m_mobile_register );
    return true;
}

//--------------------------------------------------------------------------------------------------

void net_server_t::stop_mobile_register( )
{
    ASSERT( m_mobile_register.get( ) != nullptr );

    m_device.remove_observer( *m_mobile_register );

    m_mobile_register->stop( );
    m_mobile_register->wait( );
}

//--------------------------------------------------------------------------------------------------

bool net_server_t::start_desktop_register( )
{
    ASSERT( m_desktop_register.get( ) != nullptr );

    if ( !m_desktop_register->start( ) )
    {
        LOG_ERROR( "Unable to start desktop register" );
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

void net_server_t::stop_desktop_register( )
{
    ASSERT( m_desktop_register.get( ) != nullptr );

    m_desktop_register->stop( );
    m_desktop_register->wait( );
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void net_server_t::on_notify_mobile_clients( const mobile_notification_t& notification )
{
    ASSERT( m_mobile_register.get( ) != nullptr );
    m_mobile_register->notify_clients( notification );
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void net_server_t::on_execute_desktop_command( const desktop_command_request_t& command )
{
    ASSERT( m_desktop_register.get( ) != nullptr );
    m_desktop_register->execute_command( command );
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void net_server_t::on_update_modes_request( uint modes_bitset )
{
    ASSERT( m_mobile_register.get( ) != nullptr );
    m_mobile_register->notify_modes_update( modes_bitset );
    // TODO: m_desktop_register also update
}

//--------------------------------------------------------------------------------------------------

/*virtual */
smarty::client_handler_t*
net_server_t::create_mobile_handler( socket_t& socket, const char *endpoint, packet_intf_t *hs_req )
{
    ASSERT( m_mobile_register.get( ) != nullptr );
    return new mobile_handler_t( socket, endpoint, m_config, m_device, *m_mobile_register, *this,
                                 hs_req, m_event_handler );
}

//--------------------------------------------------------------------------------------------------

/*virtual */
smarty::client_handler_t*
net_server_t::create_desktop_handler( socket_t& socket, const char *endpoint, packet_intf_t *hs_rq )
{
    ASSERT( m_desktop_register.get( ) != nullptr );
    return new desktop_handler_t( socket, endpoint, m_config, *m_desktop_register, *this, hs_rq );
}

//--------------------------------------------------------------------------------------------------
