#include <common/StdAfx.h>

#include <client/client_register.h>
#include <desktop/desktop_register.h>

#include <common/client_protocol.h>

#include <type_cast.h>


// desktop_register implementation
//--------------------------------------------------------------------------------------------------

desktop_register_t::desktop_register_t( smarty::client_register_t& clients )
    : m_desktops( )
    , m_clients_lock( )
    , m_clients( clients )
{
}

//--------------------------------------------------------------------------------------------------

desktop_register_t::~desktop_register_t( )
{
    for ( desktop_info_list_t::iterator iter = m_desktops.begin( );
          iter != m_desktops.end( ); ++iter )
    {
        on_desktop_disconnected( iter->first );

        desktop_info_t *desktop_info = iter->second;
        ASSERT( desktop_info != NULL );

        delete desktop_info;
    }

    m_desktops.clear( );
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void desktop_register_t::check_liveness( )
{
    static desktop_heartbeat_request_t request;

    mutex_locker_t lock( m_clients_lock );
    desktop_info_list_t::iterator iter = m_desktops.begin( );

    while ( iter != m_desktops.end( ) )
    {
        desktop_info_t *desktop_info = iter->second;
        if ( !desktop_info->is_alive( 2 * get_timeout( ) ) )
        {
            desktop_info_list_t::iterator temp = iter++;

            LOG_ERROR( "[desktop] [%u] is not alive. Removing. Size: %u",
                       iter->first, m_desktops.size( ) );

            m_desktops.erase( temp );
            delete desktop_info;
        }
        else
        {
            socket_t& desktop = *( desktop_info->socket );
            request.send( desktop );

            ++iter;
        }
    }
}

//--------------------------------------------------------------------------------------------------

void desktop_register_t::add_desktop( socket_t *socket, uint desktop_index, const string& name )
{
    ASSERT( socket != NULL );

    desktop_info_t *desktop_info = new desktop_info_t( socket, name );
    {
        mutex_locker_t lock( m_clients_lock );

        do_remove_desktop( desktop_index );
        m_desktops[ desktop_index ] = desktop_info;
        LOG_TRACE( "[desktop] %u added. Size: %u", desktop_index, m_desktops.size( ) );
    }

    on_desktop_connected( desktop_index, name );
}

//--------------------------------------------------------------------------------------------------

void desktop_register_t::remove_desktop( uint index )
{
    on_desktop_disconnected( index );
    {
        mutex_locker_t lock( m_clients_lock );
        do_remove_desktop( index );
    }
}

//--------------------------------------------------------------------------------------------------

void desktop_register_t::remove_desktop( socket_t *socket )
{
    mutex_locker_t lock( m_clients_lock );

    desktop_info_list_t::iterator iter = m_desktops.begin( );
    for ( ; iter != m_desktops.end( ); ++iter )
    {
        if ( iter->second->socket == socket )
        {
            on_desktop_disconnected( iter->first );
            do_remove_desktop( iter->first );
        }
    }
}

//--------------------------------------------------------------------------------------------------

void desktop_register_t::set_heartbeat_lasttime( uint index )
{
    mutex_locker_t lock( m_clients_lock );

    desktop_info_list_t::iterator iter = m_desktops.find( index );
    if ( iter != m_desktops.end( ) )
    {
        desktop_info_t& desktop_info = *( iter->second );
        desktop_info.last_time = time( NULL );
    }
}

//--------------------------------------------------------------------------------------------------

void desktop_register_t::do_remove_desktop( uint index )
{
    desktop_info_list_t::iterator iter = m_desktops.find( index );

    if ( iter != m_desktops.end( ) )
    {
        delete iter->second;
        m_desktops.erase( iter );

        LOG_TRACE( "[desktop] %u removed. Size: %u", index, m_desktops.size( ) );
    }
}

//--------------------------------------------------------------------------------------------------

void desktop_register_t::send_notification( const mobile_notification_t& notification )
{
    m_clients.on_notify_mobile_clients( notification );
}

//--------------------------------------------------------------------------------------------------

void desktop_register_t::on_desktop_connected( uint desktop_index, const string& name )
{
    mobile_notification_t notification;

    notification.type = EMN_DESKTOP_CONNECTED;
    notification.desktop_index = desktop_index;
    notification.params = name;

    LOG_TRACE( "[desktop] Type: %u, Index: %u, Params: %s",
               EMN_DESKTOP_CONNECTED, desktop_index, name.c_str( ) );

    send_notification( notification );
}

//--------------------------------------------------------------------------------------------------

void desktop_register_t::on_desktop_disconnected( uint index )
{
    mobile_notification_t notification;

    notification.type = EMN_DESKTOP_DISCONNECTED;
    notification.desktop_index = index;

    send_notification( notification );
    LOG_DEBUG( "[desktop] removed [%u]", index );
}

//--------------------------------------------------------------------------------------------------

void desktop_register_t::execute_command( const desktop_command_request_t& command )
{
    switch ( command.command )
    {
    case EDC_REFRESH_DESKTOPS:
        refresh_desktops_info( );
        break;

    default:
        execute_desktop_command( command );
    }
}

//--------------------------------------------------------------------------------------------------

void desktop_register_t::execute_desktop_command( const desktop_command_request_t& command )
{
    mutex_locker_t lock( m_clients_lock );

    uint desktop_index = command.desktop_index;
    ASSERT( desktop_index != 0 );

    desktop_info_list_t::iterator iter = m_desktops.find( desktop_index );
    if ( iter != m_desktops.end( ) )
    {
        socket_t *socket = iter->second->socket;
        ASSERT( socket != NULL );

        if ( !const_cast<desktop_command_request_t& >( command ).send( *socket ) )
        {
            LOG_ERROR( "[desktop] Could not send command to desktop %u", desktop_index );

            on_desktop_disconnected( desktop_index );
            do_remove_desktop( desktop_index );
        }
    }
    else
    {
        LOG_ERROR( "Command to unknown desktop %u. Count: %u", desktop_index, m_desktops.size( ) );
        desktop_info_list_t::iterator iter = m_desktops.begin( );
        for ( ; iter != m_desktops.end( ); ++iter )
        {
            LOG_TRACE( "%u => %s", iter->first, iter->second->name.c_str( ) );
        }
    }
}

//--------------------------------------------------------------------------------------------------

void desktop_register_t::refresh_desktops_info( )
{
    LOG_TRACE("Desktops count: %u", m_desktops.size());
    mutex_locker_t lock( m_clients_lock );

    desktop_info_list_t::const_iterator iter = m_desktops.begin( );
    for ( ; iter != m_desktops.end( ); ++iter )
    {
        on_desktop_connected( iter->first, iter->second->name );
    }
}

//--------------------------------------------------------------------------------------------------
