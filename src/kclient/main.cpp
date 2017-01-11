#include <common/StdAfx.h>

#include <common/client_lib.h>
#include <common/config_parser.h>
#include <common/client_protocol.h>

#include <signal_handler.h>


static const char g_szLoggerConfigFile[] = "logger.cfg";

//--------------------------------------------------------------------------------------------------

void light_changed( uchar state );
void notification_retrieved( mobile_notification_t *notification );

//--------------------------------------------------------------------------------------------------

int main( )
{
    LOG_INIT( "kclient", g_szLoggerConfigFile );
    //signal_handler_t::install();

    client_lib_t client;
    if ( !client.initialize( light_changed, notification_retrieved ) )
    {
        LOG_FATAL( "Unable to load client library" );
        return 1;
    }

    string error;
    if ( !client.connect( "safetylab.zapto.org", 25, "TestPswd112", error ) )
    {
        LOG_FATAL( "Unable to connect to server" );
        return 1;
    }

    config_parser_t config_parser( client.get_config( ) );
    if ( !config_parser.parse( ) )
    {
        LOG_ERROR( "Unable to parse config: \n%s", client.get_config( ) );
        return 1;
    }

    sizeint lights = config_parser.get_lights_count( );
    for ( sizeint idx = 0; idx < lights; ++idx )
    {
        light_info_t info;
        if ( config_parser.get_light_info( idx, info ) )
        {
            cout << info.pin << ". " << info.name.c_str( ) << endl;
        }
    }

    while ( true )
    {
        int command = 0, param = 0;
        cin >> command >> param;
        client.turn_light( (uchar)command, (uchar)param );
    }

    client.disconnect( );

    return 0;
}

//--------------------------------------------------------------------------------------------------

void light_changed( uchar state )
{
    LOG_INFO( "Light changed to state : %u", state );
}

//--------------------------------------------------------------------------------------------------

void notification_retrieved( mobile_notification_t *notification )
{
    ASSERT( notification != NULL );

    LOG_DEBUG( "==== Mobile notification retrieved: idx: %u, type: %u, params: %s",
        notification->desktop_index, notification->type, notification->params.c_str( ) );
}

//--------------------------------------------------------------------------------------------------
