#include <common/StdAfx.h>


#include <device/device.h>
#include <event/event_handler.h>
#include <server/net_server.h>
#include <server/smarty_server.h>
#include <common/smarty_config.h>

//#include <files/config.h>
//#include <fileutils.h>
// ignore_sigpipe
#include <net/xsocket.h>

#include <stdlib.h>
#ifdef LINUX
#	include <unistd.h>
#endif


// smarty_server_t implementation
//--------------------------------------------------------------------------------------------------

smarty_server_t::smarty_server_t( )
    : m_config( )
    , m_device( )
    , m_net_server( )
    , m_event_handler( )
{
#ifdef LINUX
    ignore_sigpipe( );
#endif
    logger::set_source("server");
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

bool smarty_server_t::start_event_handler( )
{
    ASSERT( m_config.get( ) != nullptr );

    m_event_handler.reset( new event_handler_t( *m_config, *m_device ) );
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
    ASSERT( m_net_server == nullptr );
    ASSERT( m_config != nullptr );
    ASSERT( m_device != nullptr );
    ASSERT( m_event_handler != nullptr );

    m_net_server.reset( new net_server_t( *m_config, *m_device, *m_event_handler ) );
    m_event_handler->set_client_register( m_net_server );

    if ( !m_net_server->start( ) )
    {
        ASSERT_FAIL( "Unable to start net server" );
    }

    return true;
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

    return start_device( driver ) && start_event_handler( ) && start_net_server( )
           ? ErrorCode::OK : ErrorCode::OPERATION_FAILED;
}

//--------------------------------------------------------------------------------------------------

/*virtual */
ErrorCode smarty_server_t::stop( )
{
    stop_net_server( );
    stop_event_handler( );
    stop_device( );

    return ErrorCode::OK;
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
