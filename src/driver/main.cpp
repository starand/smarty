#include <common/StdAfx.h>

#include "device_controller.h"


static device_controller_t *g_pController = NULL;

//--------------------------------------------------------------------------------------------------

extern "C"
{

//--------------------------------------------------------------------------------------------------

driver_intf_t *create_driver( )
{
    ASSERT( g_pController == NULL );
    g_pController = new (nothrow)device_controller_t( );

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
