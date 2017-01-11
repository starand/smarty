#include <common/StdAfx.h>

#include <client/client_thread.h>
#include <command/command_processor.h>
#include <desktop/desktop_handler.h>
#include <desktop/desktop_register.h>
#include <device/device_controller.h>
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


#define HANDLERS_COUNT	10


// smarty_server_t implementation
//--------------------------------------------------------------------------------------------------

smarty_server_t::smarty_server_t( )
    : m_clients_queue( )
    , m_net_server( new net_server_t( m_clients_queue ) )
    , m_device_controller( )
    , m_config( )
    , m_driver( )
    , m_command_handler( )
    , m_event_handler( )
    , m_handlers( nullptr )
    , m_mobile_register( )
    , m_desktop_register( new desktop_register_t( *this ) )
    , m_lights( )

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
    clean_clients_queue( );
}

//--------------------------------------------------------------------------------------------------

bool smarty_server_t::start_device_controller( const device_state_t& state )
{
    ASSERT( m_config.get( ) != nullptr );
    ASSERT( m_driver.get( ) != nullptr );
    ASSERT( m_command_handler.get( ) != nullptr );

    m_device_controller.reset( new device_controller_t( *m_driver, *m_config, state ) );
    if ( !m_device_controller->start( ) )
    {
        ASSERT_FAIL( "Unable to start device controller" );
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

void smarty_server_t::stop_device_controller( )
{
    ASSERT( m_device_controller.get( ) != nullptr );

    m_device_controller->stop( );
    m_device_controller->wait( );
}

//--------------------------------------------------------------------------------------------------

bool smarty_server_t::start_mobile_register( const device_state_t& state )
{
    ASSERT( m_mobile_register.get( ) == nullptr );
    ASSERT( m_driver.get( ) != nullptr );
    ASSERT( m_config.get( ) != nullptr );

    m_mobile_register.reset( new mobile_register_t( *this, *m_config, state ) );
    if ( !m_mobile_register->start( ) )
    {
        ASSERT_FAIL( "Unable to start mobile register" );
    }

    m_driver->add_observer( *m_mobile_register );
    return true;
}

//--------------------------------------------------------------------------------------------------

void smarty_server_t::stop_mobile_register( )
{
    ASSERT( m_mobile_register.get( ) != nullptr );
    ASSERT( m_driver.get( ) != nullptr );

    m_driver->remove_observer( *m_mobile_register );
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

bool smarty_server_t::start_client_handlers( )
{
    ASSERT( m_handlers == nullptr );
    m_handlers = (client_thread_t **)malloc( sizeof( client_thread_t * ) * HANDLERS_COUNT );
    for ( sizeint idx = 0; idx < HANDLERS_COUNT; ++idx )
    {
        client_thread_t *handler = new client_thread_t( *this, get_client_queue( ) );
        if ( !handler->start( ) )
        {
            ASSERT_FAIL( "Unable to start client handler" );
        }
        m_handlers[ idx ] = handler;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

void smarty_server_t::stop_client_handlers( )
{
    ASSERT( m_handlers != nullptr );

    for ( sizeint idx = 0; idx < HANDLERS_COUNT; ++idx )
    {
        client_thread_t *handler = m_handlers[ idx ];
        ASSERT( handler != nullptr );

        handler->stop( );
        handler->wait( );
    }

    free( m_handlers );
}

//--------------------------------------------------------------------------------------------------

bool smarty_server_t::start_command_handler( )
{
    ASSERT( m_config.get( ) != nullptr );
    ASSERT( m_driver.get( ) != nullptr );

    m_command_handler.reset( new command_processor_t( *m_driver, *m_config, m_lights ) );
    if ( !m_command_handler->start( ) )
    {
        ASSERT_FAIL( "Unable to start command handler" );
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

void smarty_server_t::stop_command_handler( )
{
    ASSERT( m_command_handler.get( ) != nullptr );

    m_command_handler->stop( );
    m_command_handler->wait( );
}

//--------------------------------------------------------------------------------------------------

bool smarty_server_t::start_event_handler( const device_state_t& state )
{
    ASSERT( m_config.get( ) != nullptr );
    ASSERT( m_driver.get( ) != nullptr );

    m_event_handler.reset( new event_handler_t( *m_config, *m_command_handler, state ) );
    if ( !m_event_handler->init( ) )
    {
        ASSERT_FAIL( "Unable to start event handler" );
    }

    m_driver->add_observer( *m_event_handler );
    return true;
}

//--------------------------------------------------------------------------------------------------

void smarty_server_t::stop_event_handler( )
{
    ASSERT( m_event_handler.get( ) != nullptr );
    ASSERT( m_driver.get( ) != nullptr );

    m_driver->remove_observer( *m_event_handler );
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

void smarty_server_t::clean_clients_queue( )
{
    while ( !m_clients_queue.empty( ) )
    {
        socket_t* client = m_clients_queue.pop( );
        FREE_POINTER( client );
    }
}

//--------------------------------------------------------------------------------------------------

bool smarty_server_t::init_light_objects( )
{
    auto lights_node = ( *m_config )[ "lights" ];
    if ( lights_node.isNull( ) )
    {
        LOG_ERROR( "Lights node not set in config" );
        return false;
    }

    uint lights_count = lights_node.size( );
    ASSERT( lights_count != 0 );
    m_lights.resize( lights_count );

    return true;
}

//--------------------------------------------------------------------------------------------------

/*virtual */
ErrorCode smarty_server_t::start( std::shared_ptr< driver_intf_t > driver,
                                  std::shared_ptr< smarty_config_t > config )
{
    m_driver = driver;
    m_config = config;

    // to get real state we have to execute command
    device_command_t status_command = { EC_STATUS, 0x00 };
    m_driver->execute_command( status_command );

    device_state_t state;
    if ( m_driver->get_state( state ) != ErrorCode::OK )
    {
        LOG_ERROR( "Unable to retreive device state" );
        return ErrorCode::OPERATION_FAILED;
    }

    if ( !init_light_objects( ) )
    {
        return ErrorCode::OPERATION_FAILED;;
    }

    return start_command_handler( ) && start_event_handler( state ) &&
           start_device_controller( state ) && start_mobile_register( state ) &&
           start_desktop_register( ) && start_client_handlers( ) && start_net_server( )
           ? ErrorCode::OK : ErrorCode::OPERATION_FAILED;
}

//--------------------------------------------------------------------------------------------------

/*virtual */
ErrorCode smarty_server_t::stop( )
{
    stop_net_server( );
    stop_client_handlers( );
    stop_desktop_register( );
    stop_mobile_register( );
    stop_device_controller( );
    stop_event_handler( );
    stop_command_handler( );

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
void smarty_server_t::on_update_modes_request( const update_modes_request_t& request )
{
    m_event_handler->update_modes( request.state );
    m_mobile_register->notify_modes_update( m_event_handler->get_modes_bitset( ) );
}

//--------------------------------------------------------------------------------------------------

/*virtual */
smarty::client_handler_t*
smarty_server_t::create_mobile_handler( socket_t& socket, const char *endpoint,
                                        packet_intf_t *hs_req )
{
    ASSERT( m_config.get( ) != nullptr );
    ASSERT( m_mobile_register.get( ) != nullptr );

    return new mobile_handler_t( socket, endpoint, *m_config, *m_device_controller,
                                 *m_mobile_register, *this, hs_req, *m_command_handler,
                                 *m_event_handler );
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