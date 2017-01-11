#include <common/StdAfx.h>
#include "webserver_module.h"

#include <library.h>


static const char g_szServerLibName[] = "webserver.so";
static const char g_szGetServer[] = "get_server";

typedef web_server_intf_t* ( *get_server_funct_t )( );


//--------------------------------------------------------------------------------------------------

webserver_module_t::webserver_module_t( )
    : library_intf_t( g_szServerLibName )
    , m_server( nullptr )
{
}

//--------------------------------------------------------------------------------------------------

webserver_module_t::~webserver_module_t( )
{
}

//--------------------------------------------------------------------------------------------------

/*virtual */
ErrorCode webserver_module_t::start( std::shared_ptr< driver_intf_t > driver,
                              std::shared_ptr< smarty_config_t > config )
{
    if ( !is_loaded( ) )
    {
        return ErrorCode::NOT_LOADED;
    }

    return m_server->start( driver, config );
}

//--------------------------------------------------------------------------------------------------

/*virtual */
ErrorCode webserver_module_t::stop( )
{
    if ( !is_loaded( ) )
    {
        return ErrorCode::NOT_LOADED;
    }

    return m_server->stop( );
}

//--------------------------------------------------------------------------------------------------

ErrorCode webserver_module_t::load( )
{
    if ( !library_intf_t::load( ) || !load_functions( ) )
    {
        return ErrorCode::NOT_LOADED;
    }

    m_loaded = true;
    return ErrorCode::OK;
}

//--------------------------------------------------------------------------------------------------

ErrorCode webserver_module_t::unload( )
{
    ASSERT( m_lib != nullptr );
    return m_lib->unload( ) ? ErrorCode::OK : ErrorCode::OPERATION_FAILED;
}

//--------------------------------------------------------------------------------------------------

/*virtual */
bool webserver_module_t::is_loaded( ) const
{
    return m_loaded;
}

//--------------------------------------------------------------------------------------------------

/*virtual */
bool webserver_module_t::load_functions( )
{
    ASSERT( m_lib );

    get_server_funct_t get_server = (get_server_funct_t)m_lib->load_function( g_szGetServer );
    ASSERT( get_server != NULL );

    m_server = get_server( );
    ASSERT( m_server );

    return true;
}

//--------------------------------------------------------------------------------------------------
