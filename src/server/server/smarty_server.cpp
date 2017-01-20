#include <common/StdAfx.h>

#include <desktop/desktop_handler.h>
#include <desktop/desktop_register.h>
#include <device/device.h>
#include <event/event_handler.h>
#include <mobile/mobile_handler.h>
#include <mobile/mobile_register.h>
#include <server/net_server.h>
#include <server/smarty_server.h>

#include <common/smarty_config.h>
#include <common/client_protocol.h>
#include <files/config.h>

#include <xsocket.h>
#include <fileutils.h>

#include <stdlib.h>
#ifdef LINUX
#	include <unistd.h>
#endif


// smarty_server_t implementation
//--------------------------------------------------------------------------------------------------

smarty_server_t::smarty_server_t( )
    : m_net_server( new net_server_t( *this ) )
    , m_device( )
    , m_config( )
    , m_event_handler( )

    , m_mobile_register( )
    , m_desktop_register( new desktop_register_t( *this ) )
{
#ifdef LINUX
    ignore_sigpipe( );
#endif
    logger::set_source("server");

    ASSERT( m_net_server.get( ) != nullptr );
    ASSERT( m_desktop_register.get( ) != nullptr );
}

//--------------------------------------------------------------------------------------------------

smarty_server_t::~smarty_server_t( )
{
}

//--------------------------------------------------------------------------------------------------

bool smarty_server_t::start_device( std::shared_ptr< driver_intf_t > driver )
{
    ASSERT( m_config.get( ) != nullptr );
    ASSERT( driver.get( ) != nullptr );

    m_device.reset( new device_t( *driver, *m_config ) );
    if ( !m_device->init( ) )
    {
        ASSERT_FAIL( "Unable to init device" );
    }

    if ( !m_device->start( ) )
    {
        ASSERT_FAIL( "Unable to start device" );
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

void smarty_server_t::stop_device( )
{
    ASSERT( m_device.get( ) != nullptr );

    m_device->stop( );
    m_device->wait( );
}

//--------------------------------------------------------------------------------------------------

bool smarty_server_t::start_mobile_register( )
{
    ASSERT( m_mobile_register.get( ) == nullptr );
    ASSERT( m_config.get( ) != nullptr );

    m_mobile_register.reset( new mobile_register_t( *this, *m_config ) );
    if ( !m_mobile_register->start( ) )
    {
        ASSERT_FAIL( "Unable to start mobile register" );
    }

    m_device->add_observer( *m_mobile_register );
    return true;
}

//--------------------------------------------------------------------------------------------------

void smarty_server_t::stop_mobile_register( )
{
    ASSERT( m_mobile_register.get( ) != nullptr );

    m_device->remove_observer( *m_mobile_register );
    m_mobile_register->stop( );
    m_mobile_register->wait( );
}

//--------------------------------------------------------------------------------------------------

bool smarty_server_t::start_desktop_register( )
{
    if ( !m_desktop_register->start( ) )
    {
        ASSERT_FAIL( "Unable to start desktop register" );
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

void smarty_server_t::stop_desktop_register( )
{
    m_desktop_register->stop( );
    m_desktop_register->wait( );
}

//--------------------------------------------------------------------------------------------------

bool smarty_server_t::start_event_handler( )
{
    ASSERT( m_config.get( ) != nullptr );

    m_event_handler.reset( new event_handler_t( *m_config, *m_device, *this ) );
    if ( !m_event_handler->init( ) )
    {
        ASSERT_FAIL( "Unable to init event handler" );
    }

    if ( !m_event_handler->start( ) )
    {
        ASSERT_FAIL( "Unable to start event handler" );
    }

    m_device->add_observer( *m_event_handler );
    return true;
}

//--------------------------------------------------------------------------------------------------

void smarty_server_t::stop_event_handler( )
{
    ASSERT( m_event_handler.get( ) != nullptr );

    m_device->remove_observer( *m_event_handler );

    m_event_handler->stop( );
    m_event_handler->wait( );
}

//--------------------------------------------------------------------------------------------------

bool smarty_server_t::start_net_server( )
{
    bool success = m_net_server->start( );
    if ( !success )
    {
        ASSERT_FAIL( "Unable to start net server" );
    }

    return success;
}

//--------------------------------------------------------------------------------------------------

void smarty_server_t::stop_net_server( )
{
    m_net_server->stop( );
    m_net_server->wait( );
}

//--------------------------------------------------------------------------------------------------

/*virtual */
ErrorCode smarty_server_t::start( std::shared_ptr< driver_intf_t > driver,
                                  std::shared_ptr< smarty_config_t > config )
{
    m_config = config;

    return start_device( driver ) && start_event_handler( ) && start_mobile_register( ) && 
           start_desktop_register( ) && start_net_server( )
           ? ErrorCode::OK : ErrorCode::OPERATION_FAILED;
}

//--------------------------------------------------------------------------------------------------

/*virtual */
ErrorCode smarty_server_t::stop( )
{
    stop_net_server( );
    stop_desktop_register( );
    stop_mobile_register( );
    stop_device( );
    stop_event_handler( );

    return ErrorCode::OK;
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void smarty_server_t::on_notify_mobile_clients( const mobile_notification_t& notification )
{
    ASSERT( m_mobile_register.get( ) != nullptr );
    m_mobile_register->notify_clients( notification );
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void smarty_server_t::on_execute_desktop_command( const desktop_command_request_t& command )
{
    m_desktop_register->execute_command( command );
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void smarty_server_t::on_update_modes_request( uint modes_bitset )
{
    m_mobile_register->notify_modes_update( modes_bitset );
    // TODO: m_desktop_register also update
}

//--------------------------------------------------------------------------------------------------

/*virtual */
smarty::client_handler_t*
smarty_server_t::create_mobile_handler( socket_t& socket, const char *endpoint,
                                        packet_intf_t *hs_req )
{
    ASSERT( m_config.get( ) != nullptr );
    ASSERT( m_mobile_register.get( ) != nullptr );

    std::string ep( endpoint );
    return new mobile_handler_t( socket, ep, *m_config, *m_device, *m_mobile_register, *this,
                                 hs_req, *m_event_handler );


}

//--------------------------------------------------------------------------------------------------

/*virtual */
smarty::client_handler_t*
smarty_server_t::create_desktop_handler( socket_t& socket, const char *endpoint,
                                         packet_intf_t *hs_req )
{
    ASSERT( m_config.get( ) != nullptr );

    return new desktop_handler_t( socket, endpoint, *m_config, *m_desktop_register, *this, hs_req );
}

//--------------------------------------------------------------------------------------------------

const event_handler_t&
smarty_server_t::get_event_handler( ) const
{
    return *m_event_handler;
}

//--------------------------------------------------------------------------------------------------
// PUBLIC INTERFACE
//--------------------------------------------------------------------------------------------------

smarty_server_t g_smarty_server;

extern "C"
{
    server_intf_t *get_server( )
    {
        logger::set_source( "server" );
        return& g_smarty_server;
    }
};

//--------------------------------------------------------------------------------------------------
