#include <common/StdAfx.h>
#include "smarty_app.h"

#include "driver_module.h"
#include "server_module.h"

#include <common/config.h>

#include <fileutils.h>
#include <utils.h>

#ifdef LINUX
#   include <signal.h>
#   include <unistd.h>
#endif


static const char g_szLoggerConfigDef[] = "logger.cfg";
static const char g_szSmartyConfigDef[] = "smarty.cfg";

volatile bool smarty_app_t::m_stopping = false;

//--------------------------------------------------------------------------------------------------

smarty_app_t::smarty_app_t( )
    : m_driver( )
    , m_server( )
    , m_binary_dir( )
    , m_config( new config_t( ) )
    , m_logger( )
{
    ASSERT( m_config.get( ) != nullptr );
}

//--------------------------------------------------------------------------------------------------

smarty_app_t::~smarty_app_t( )
{
}

//--------------------------------------------------------------------------------------------------

int smarty_app_t::run( )
{
    if ( !load_configs( ) )
    {
        return 1;
    }

    if ( !load_plugins( ) )
    {
        return 2;
    }

#ifdef LINUX
    install_signal_handlers( );
#endif

    do_run( );

    unload_plugins( );

    return 0;
}

//--------------------------------------------------------------------------------------------------

void smarty_app_t::do_run( )
{
    while ( !m_stopping )
    {
        utils::sleep_ms( 1000 );
    }
}

//--------------------------------------------------------------------------------------------------

bool smarty_app_t::get_config_filenames( )
{
    if ( !FileUtils::GetBinaryDir( m_binary_dir ) )
    {
        LOG_FATAL( "Could not retrieve smarty binary path" );
        return false;
    }

    string logger_config_filename = m_binary_dir + g_szLoggerConfigDef;
    m_logger.reset( new logger_initializer_t( "smarty", logger_config_filename.c_str( ) ) );

    return true;
}

//--------------------------------------------------------------------------------------------------

bool smarty_app_t::load_config( )
{
    string config_file = m_binary_dir + g_szSmartyConfigDef;
    if ( !m_config->read_config( config_file ) )
    {
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

bool smarty_app_t::load_configs( )
{
    return get_config_filenames( ) && load_config( );
}

//--------------------------------------------------------------------------------------------------

bool smarty_app_t::load_driver( )
{
    m_driver = make_shared< driver_module_t >( );

    ErrorCode error_code = m_driver->load( );
    if ( error_code != ErrorCode::OK )
    {
        LOG_FATAL( "Unable to load driver. Error code: %u", error_code );
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

bool smarty_app_t::load_server( )
{
    m_server.reset( new server_module_t( ) );

    ErrorCode error_code = m_server->load( );
    if ( error_code != ErrorCode::OK )
    {
        LOG_FATAL( "Unable to load server. Error code: %u", error_code );
        return false;
    }

    error_code = m_server->start( m_driver, m_config );
    if ( error_code != ErrorCode::OK )
    {
        LOG_FATAL( "Unable to start server. Error code: %u", error_code );
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

bool smarty_app_t::load_plugins( )
{
    return load_driver( ) && load_server( );
}

//--------------------------------------------------------------------------------------------------

void smarty_app_t::unload_driver( )
{
    m_driver->unload( );
    m_driver.reset( );
}

//--------------------------------------------------------------------------------------------------

void smarty_app_t::unload_server( )
{
    m_server->stop( );
    m_server->unload( );
    m_server.reset( );
}

//--------------------------------------------------------------------------------------------------

void smarty_app_t::unload_plugins( )
{
    unload_server( );
    unload_driver( );
}

//--------------------------------------------------------------------------------------------------
// SIGNAL HANDLERS
//--------------------------------------------------------------------------------------------------

#ifdef LINUX

static void handler( int sig )
{
    LOG_INFO( "Signal received. Stopping .." );
    smarty_app_t::m_stopping = true;
}

//--------------------------------------------------------------------------------------------------

void smarty_app_t::install_signal_handlers( )
{
    struct sigaction sa;
    sigemptyset( &sa.sa_mask );
    sa.sa_flags = 0;
    sa.sa_handler = handler;

    int res = sigaction( SIGINT, &sa, NULL );
    ASSERT( res != -1 );
    res = sigaction( SIGTERM, &sa, NULL );
    ASSERT( res != -1 );
}

#endif // LINUX

//--------------------------------------------------------------------------------------------------
// PROGRAM ENTRY POINT
//--------------------------------------------------------------------------------------------------

int main( )
{
    return smarty_app_t( ).run( );
}
