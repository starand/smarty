#include <common/StdAfx.h>

#include "device_controller.h"
#include "device_driver.h"

#include <memory>


static device_controller_t *g_pController = NULL;
static std::unique_ptr< device_driver_t > driver;

//--------------------------------------------------------------------------------------------------

extern "C"
{

//--------------------------------------------------------------------------------------------------

driver_intf_t *create_driver( )
{
    ASSERT( g_pController == NULL );

    driver.reset( new device_driver_t( ) );
    g_pController = new (nothrow)device_controller_t( *driver.get( ) );

    bool stated = g_pController->start( );
    ASSERT( stated == true );

    return g_pController;
}

//--------------------------------------------------------------------------------------------------

void free_driver( )
{
    if ( g_pController )
    {
        delete g_pController;
        g_pController = NULL;
    }
}

//--------------------------------------------------------------------------------------------------

};
