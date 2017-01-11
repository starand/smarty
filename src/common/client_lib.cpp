#include <common/StdAfx.h>
#include "client_lib.h"

#include <library.h>


#ifdef WINDOWS
static const char g_szLibName[] = "client.dll";
#else
static const char g_szLibName[] = "libclient.so";
#endif


static const char g_szInitializeFunct[] = "initialize";
static const char g_szFinalizeFunct[] = "finalize";
static const char g_szTurnLight[] = "turn_light";
static const char g_szDesktopCommand[] = "desktop_command";
static const char g_szServerCommand[] = "server_command";
static const char g_szGetLastError[] = "get_last_error";
static const char g_szGetConfig[] = "get_config";
static const char g_szSetLoggers[] = "set_loggers";


//////////////////////////////////////////////////

static client_lib_t *g_active_client_instance = NULL;

//--------------------------------------------------------------------------------------------------

void light_change_callback( uchar state )
{
    if ( g_active_client_instance )
    {
        g_active_client_instance->on_light_changed( state );
    }
}

//--------------------------------------------------------------------------------------------------

void notification_callback( mobile_notification_t *notification )
{
    if ( g_active_client_instance )
    {
        g_active_client_instance->on_notification( notification );
    }
}

//--------------------------------------------------------------------------------------------------

//////////////////////////////////////////////////
// client_lib_t implementation

client_lib_t::client_lib_t( )
    : library_intf_t( g_szLibName )
    , m_initialize( NULL )
    , m_finalize( NULL )
    , m_turn_light( NULL )
    , m_desktop_command( NULL )
    , m_get_last_error( NULL )
    , m_get_config( NULL )
    , m_set_loggers( NULL )
    , m_light_changed_callback( NULL )
    , m_notification_callback( NULL )
    , m_connected( false )
{
}

//--------------------------------------------------------------------------------------------------

/*virtual */
client_lib_t::~client_lib_t( )
{

}

//--------------------------------------------------------------------------------------------------

bool client_lib_t::initialize( change_state_callback_f light_changed_callback,
                               notification_callback_f notification_callback )
{
    if ( !library_intf_t::load( ) )
    {
        return false;
    }

    m_light_changed_callback = light_changed_callback;
    m_notification_callback = notification_callback;

    if ( !m_lib )
    {
        return false;
    }

    if ( !load_functions( ) )
    {
        return false;
    }

    m_set_loggers( logger::get_source( ), logger::get_loggers( ) );

    m_loaded = true;
    return true;
}

//--------------------------------------------------------------------------------------------------

/*virtual */
bool client_lib_t::load_functions( )
{
    ASSERT( m_lib != NULL );
    m_initialize = (initialize_funct_t)m_lib->load_function( g_szInitializeFunct );
    m_finalize = (finalize_funct_t)m_lib->load_function( g_szFinalizeFunct );

    m_turn_light = (turn_light_funct_t)m_lib->load_function( g_szTurnLight );
    m_desktop_command = (desktop_command_funct_t)m_lib->load_function( g_szDesktopCommand );
    m_server_command = (server_command_funct_t)m_lib->load_function( g_szServerCommand );

    m_get_last_error = (get_last_error_funct_t)m_lib->load_function( g_szGetLastError );
    m_get_config = (get_config_funct_t)m_lib->load_function( g_szGetConfig );
    m_set_loggers = (set_loggers_funct_t)m_lib->load_function( g_szSetLoggers );

    return m_initialize && m_finalize && m_turn_light && m_desktop_command
           && m_get_last_error && m_get_config && m_set_loggers;
}

//--------------------------------------------------------------------------------------------------

bool client_lib_t::connect( const char *host, ushort port, const char* password, string& error )
{
    g_active_client_instance = this;
    ASSERT( m_initialize != NULL ); ASSERT( m_get_last_error != NULL );

    if ( !m_initialize( host, port, password, light_change_callback, notification_callback ) )
    {
        error = m_get_last_error( );
        return false;
    }

    m_connected = true;
    return true;
}

//--------------------------------------------------------------------------------------------------

void client_lib_t::disconnect( )
{
    g_active_client_instance = NULL;

    ASSERT( m_connected );
    ASSERT( m_finalize != NULL );
    m_finalize( );
}

//--------------------------------------------------------------------------------------------------

bool client_lib_t::is_connected( ) const
{
    return m_connected;
}

//--------------------------------------------------------------------------------------------------

bool client_lib_t::turn_light( uchar command, uchar param )
{
    ASSERT( m_connected );
    ASSERT( m_turn_light != NULL );
    return m_turn_light( command, param );
}

//--------------------------------------------------------------------------------------------------

void client_lib_t::desktop_command( const desktop_command_request_t& command )
{
    ASSERT( m_connected );
    ASSERT( m_desktop_command != NULL );

    m_desktop_command( command );
}

//--------------------------------------------------------------------------------------------------

ErrorCode client_lib_t::server_command( const server_command_request_t& cmd )
{
    ASSERT( m_connected );
    ASSERT( m_server_command != nullptr );

    return m_server_command( cmd );
}

//--------------------------------------------------------------------------------------------------

const char *client_lib_t::get_config( ) const
{
    return m_get_config( );
}

//--------------------------------------------------------------------------------------------------

void client_lib_t::on_light_changed( uchar state )
{
    if ( m_light_changed_callback )
    {
        m_light_changed_callback( state );
    }
}

//--------------------------------------------------------------------------------------------------

void client_lib_t::on_notification( mobile_notification_t *notification )
{
    if ( m_notification_callback )
    {
        m_notification_callback( notification );
    }
}

//--------------------------------------------------------------------------------------------------
