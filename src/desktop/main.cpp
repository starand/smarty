#include <common/StdAfx.h>

#include "desktop.h"

#include <config.h>


static const char g_szConfigFileName[] = "desktop.cfg";
static const char g_szLoggerConfig[] = "logger.cfg";

//--------------------------------------------------------------------------------------------------

int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    LOG_INIT( "desktop", g_szLoggerConfig );

    config_t config;
    if ( !config.read_config( g_szConfigFileName, true ) )
    {
        std::cout << "desktop.cfg not found" << std::endl
                  << "Config sample:" << std::endl
                  << "server = 10.0.0.9\nport = 53074\npassword = TestPassword\nmp_folder = music"
                  << std::endl;
        return -1;
    }

    desktop_t desktop( config );
    desktop.run( );

    return 0;
}

//--------------------------------------------------------------------------------------------------
