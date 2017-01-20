#include <common/StdAfx.h>

#include <client/client_linker.h>
#include <common/client_protocol.h>
#include <mobile/mobile_register.h>

#include <files/config.h>
#include <net/xsocket.h>

#include <json/writer.h>


// mobile_register_t implementation
//--------------------------------------------------------------------------------------------------

mobile_register_t::mobile_register_t( smarty::client_linker_t& connector, const config_t& config )
    : m_desktop_connector( connector )
    , m_client_list( )
    , m_client_list_mutex( )
    , m_prev_state( )
    , m_config( config )
{
}

//--------------------------------------------------------------------------------------------------

mobile_register_t::~mobile_register_t( )
{
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void mobile_register_t::check_liveness( )
{
    static mobile_heartbeat_request_t request;

    mutex_locker_t lock( m_client_list_mutex );

    client_list_t::iterator itList = m_client_list.begin( );
    while ( itList != m_client_list.end( ) )
    {
        socket_t *socket = itList->socket;

        if ( !itList->is_alive( 2 * get_timeout( ) ) )
        {
            drop_client( itList );
        }
        else
        {
            request.send( *socket );
            ++itList;
        }
    }
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void mobile_register_t::on_light_changed( const lights_state_t& state )
{
    if ( !m_client_list.empty( ) )
    {
        update_clients( state );
    }
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void mobile_register_t::on_button_pressed( const buttons_state_t& state ) { /* stub */ }

//--------------------------------------------------------------------------------------------------

/*virtual */
void mobile_register_t::on_sensor_triggered( const sensors_state_t& state ) { /* stub */ }

//--------------------------------------------------------------------------------------------------

/*virtual */
void mobile_register_t::on_double_click( uint button_pin ) { /* stub */ }

//--------------------------------------------------------------------------------------------------

bool mobile_register_t::update_config( socket_t& client )
{
    config_update_response_t config_update;

    auto lights_node = m_config[ "lights" ];
    if ( lights_node.isNull( ) )
    {
        LOG_ERROR( "Lights node does not exist in config" );
        return false;
    }

    auto modes = m_config[ "modes" ];
    if ( modes.isNull( ) )
    {
        LOG_ERROR( "Modes node does not exist in config" );
        return false;
    }

    Json::Value root;
    root[ "lights" ] = lights_node;
    root[ "modes" ] = modes;

    Json::StyledStreamWriter writer;
    config_update.config = Json::FastWriter( ).write( root );

    if ( !config_update.send( client ) )
    {
        return false;
    }

    LOG_TRACE( "[mobile] Client config sent: %s", config_update.config.c_str( ) );
    return true;
}

//--------------------------------------------------------------------------------------------------

void mobile_register_t::add_client( socket_t *client, const string& end_point )
{
    ASSERT( client != NULL );

    {
        mutex_locker_t lock( m_client_list_mutex );
        m_client_list.push_back( client_info_t( end_point, client ) );
    }

    on_client_connected( );
}

//--------------------------------------------------------------------------------------------------

void mobile_register_t::remove_client( socket_t *socket )
{
    ASSERT( socket != NULL );
    mutex_locker_t lock( m_client_list_mutex );

    client_list_t::iterator iter = find_mobile( socket );
    if ( iter != m_client_list.end( ) )
    {
        m_client_list.erase( iter );
    }
}

//--------------------------------------------------------------------------------------------------

void mobile_register_t::notify_clients( const mobile_notification_t& notification )
{
    mutex_locker_t lock( m_client_list_mutex );
    client_list_t::iterator itList = m_client_list.begin( );

    while ( itList != m_client_list.end( ) )
    {
        socket_t *client = itList->socket; ASSERT( client != NULL );
        if ( !const_cast<mobile_notification_t& >( notification ).send( *client ) )
        {
            drop_client( itList );
        }
        else
        {
            ++itList;
        }
    }
}

//--------------------------------------------------------------------------------------------------

void mobile_register_t::notify_modes_update( uint modes )
{
    mutex_locker_t lock( m_client_list_mutex );
    client_list_t::iterator itList = m_client_list.begin( );

    while ( itList != m_client_list.end( ) )
    {
        modes_update_notification_t notification;
        notification.state = modes;

        socket_t *client = itList->socket; ASSERT( client != NULL );
        if ( !notification.send( *client ) )
        {
            drop_client( itList );
        }
        else
        {
            ++itList;
        }
    }
}

//--------------------------------------------------------------------------------------------------

void mobile_register_t::set_heartbeat_lasttime( socket_t *socket )
{
    client_list_t::iterator iter = find_mobile( socket );

    if ( iter != m_client_list.end( ) )
    {
        iter->last_time = time( NULL );
    }
}

//--------------------------------------------------------------------------------------------------

void mobile_register_t::update_clients( const lights_state_t& state )
{
    light_notification_t notification;
    notification.lights_state = static_cast< uchar >( state );

    mutex_locker_t lock( m_client_list_mutex );
    client_list_t::iterator itList = m_client_list.begin( );

    while ( itList != m_client_list.end( ) )
    {
        socket_t *client = itList->socket;
        if ( !notification.send( *client ) )
        {
            drop_client( itList );
        }
        else
        {
            if ( m_prev_state.lights != notification.lights_state )
            {
                LOG_TRACE( "[mobile] Light notification with state %u sent to %s",
                           notification.lights_state, itList->endpoint.c_str( ) );
            }

            ++itList;
        }
    }

    m_prev_state.lights = state;
}

//--------------------------------------------------------------------------------------------------

void mobile_register_t::execute_desktop_command( const desktop_command_request_t& command )
{
    m_desktop_connector.on_execute_desktop_command( command );
}

//--------------------------------------------------------------------------------------------------

void mobile_register_t::on_client_connected( )
{
    desktop_command_request_t command;
    command.desktop_index = 0;
    command.command = EDC_REFRESH_DESKTOPS;

    execute_desktop_command( command );
}

//--------------------------------------------------------------------------------------------------

client_list_t::iterator mobile_register_t::find_mobile( socket_t *socket )
{
    client_list_t::iterator iter = m_client_list.begin( );

    for ( ; iter != m_client_list.end( ); ++iter )
    {
        if ( iter->socket == socket )
        {
            break;
        }
    }

    return iter;
}

//--------------------------------------------------------------------------------------------------

void mobile_register_t::drop_client( client_list_t::iterator& itList )
{
    socket_t *socket = itList->socket; ASSERT( socket != NULL );

    socket->close( );
    LOG_ERROR( "[mobile] client %s was dropped", itList->endpoint.c_str( ) );

    itList = m_client_list.erase( itList );
}

//--------------------------------------------------------------------------------------------------
