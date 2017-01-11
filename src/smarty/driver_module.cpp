#include <common/StdAfx.h>
#include "driver_module.h"

#include <library.h>


static const char g_szDriverLibName[] = "driver.so";

static const char g_szCreateDriver[] = "create_driver";
static const char g_szFreeDriver[] = "free_driver";


//--------------------------------------------------------------------------------------------------

driver_module_t::driver_module_t( )
    : library_intf_t( g_szDriverLibName )
    , m_create_driver( nullptr )
    , m_free_driver( nullptr )
    , m_driver( nullptr )
{
}

//--------------------------------------------------------------------------------------------------

driver_module_t::~driver_module_t( )
{
    m_driver = nullptr;

    if ( m_free_driver )
    {
        m_free_driver( );
        m_free_driver = nullptr;
        m_create_driver = nullptr;
    }
}

//--------------------------------------------------------------------------------------------------

ErrorCode driver_module_t::load( )
{
    if ( !library_intf_t::load( ) || !load_functions( ) )
    {
        return ErrorCode::NOT_LOADED;
    }

    m_driver = m_create_driver( );
    if ( m_driver == nullptr )
    {
        return ErrorCode::OPERATION_FAILED;
    }

    m_loaded = true;
    return ErrorCode::OK;
}

//--------------------------------------------------------------------------------------------------

ErrorCode driver_module_t::unload( )
{
    ASSERT( m_lib != nullptr );
    return m_lib->unload( ) ? ErrorCode::OK : ErrorCode::OPERATION_FAILED;
}

//--------------------------------------------------------------------------------------------------

/*virtual */
bool driver_module_t::is_loaded( ) const
{
    return m_loaded;
}

//--------------------------------------------------------------------------------------------------

/*virtual */
bool driver_module_t::load_functions( )
{
    ASSERT( m_lib );

    ASSERT( m_create_driver == nullptr );
    m_create_driver = (create_driver_funct_t)m_lib->load_function( g_szCreateDriver );
    if ( !m_create_driver ) return false;

    ASSERT( m_free_driver == nullptr );
    m_free_driver = (free_driver_funct_t)m_lib->load_function( g_szFreeDriver );
    if ( !m_free_driver ) return false;

    return true;
}

//--------------------------------------------------------------------------------------------------

/*virtual */
ErrorCode driver_module_t::execute_command( const device_command_t& command )
{
    if ( !is_loaded( ) )
    {
        return ErrorCode::NOT_LOADED;
    }

    ASSERT( m_driver != nullptr );
    return m_driver->execute_command( command );
}

//--------------------------------------------------------------------------------------------------

/*virtual */
ErrorCode driver_module_t::get_state( device_state_t& state ) const
{
    if ( !is_loaded( ) )
    {
        return ErrorCode::NOT_LOADED;
    }

    ASSERT( m_driver != nullptr );
    return m_driver->get_state( state );
}

//--------------------------------------------------------------------------------------------------

/*virtual */
ErrorCode driver_module_t::add_observer( device_observer_t& observer )
{
    if ( !is_loaded( ) )
    {
        return ErrorCode::NOT_LOADED;
    }

    ASSERT( m_driver != nullptr );
    return m_driver->add_observer( observer );
}

//--------------------------------------------------------------------------------------------------

/*virtual */
ErrorCode driver_module_t::remove_observer( device_observer_t& observer )
{
    if ( !is_loaded( ) )
    {
        return ErrorCode::NOT_LOADED;
    }

    ASSERT( m_driver != nullptr );
    return m_driver->remove_observer( observer );
}

//--------------------------------------------------------------------------------------------------
