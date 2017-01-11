#include <common/StdAfx.h>
#include <common/libclient_interface.h>

#include "smarty_connector.h"


smarty_connector_t *g_connector = NULL;

//--------------------------------------------------------------------------------------------------

extern "C"
{
//--------------------------------------------------------------------------------------------------

bool initialize( const char *host, ushort port, const char *password,
                 change_state_callback_f change_state_callback,
                 notification_callback_f notification_callback )
{
    ASSERT( g_connector == NULL );
    g_connector = new (nothrow)smarty_connector_t( change_state_callback, notification_callback );
    ASSERT( g_connector != NULL );

    if ( !g_connector->connect( host, port, password ) )
    {
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

void finalize( )
{
    ASSERT( g_connector != NULL );
    g_connector->disconnect( );
    FREE_POINTER( g_connector );
}

//--------------------------------------------------------------------------------------------------

void turn_light( uchar command, uchar param )
{
    ASSERT( g_connector != NULL );
    g_connector->turn_light( command, param );
}

//--------------------------------------------------------------------------------------------------

void desktop_command( const desktop_command_request_t& command )
{
    ASSERT( g_connector != NULL );
    g_connector->desktop_command( command );
}

//--------------------------------------------------------------------------------------------------

ErrorCode server_command( const server_command_request_t& cmd )
{
    ASSERT( g_connector != nullptr );
    return g_connector->server_command( cmd );
}

//--------------------------------------------------------------------------------------------------

const char *get_last_error( )
{
    return g_connector->get_last_error( );
}

//--------------------------------------------------------------------------------------------------

const char *get_config( )
{
    return g_connector->get_config( );
}

//--------------------------------------------------------------------------------------------------

void set_loggers( const char *source, void *loggers )
{
    logger::set_loggers( source, loggers );
}

//--------------------------------------------------------------------------------------------------

};
