#include <common/StdAfx.h>
#include "library_intf.h"

#include <library.h>

#ifdef LINUX
#	include <dlfcn.h>
#endif


//////////////////////////////////////////////////
// library_intf_t implementation
//--------------------------------------------------------------------------------------------------

library_intf_t::library_intf_t( const char *lib_name )
    : m_lib( NULL )
    , m_name( lib_name )
    , m_loaded( false )
{
    ASSERT( lib_name != NULL );
}

//--------------------------------------------------------------------------------------------------

library_intf_t::~library_intf_t( )
{
    if ( m_lib )
    {
        m_lib->unload( );

        delete m_lib;
        m_lib = NULL;
    }

    m_loaded = false;
}

//--------------------------------------------------------------------------------------------------

bool library_intf_t::load( )
{
    m_lib = new (nothrow)library_t( );

    if ( !m_lib->load( m_name.c_str( ) ) )
    {
        LOG_ERROR( "Unable to load library \'%s\'", m_name.c_str( ) );
#ifdef LINUX
        LOG_DEBUG( dlerror( ) );
#endif
        delete m_lib;
        m_lib = NULL;
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

/*virtual */
bool library_intf_t::is_loaded( ) const
{
    return m_lib != NULL;
}

//--------------------------------------------------------------------------------------------------
