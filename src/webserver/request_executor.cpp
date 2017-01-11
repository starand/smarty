#include <common/StdAfx.h>
#include "request_executor.h"

#include <common/driver_intf.h>

#include <strutils.h>


static const char g_szLightsNode[] = "light";
static const char g_szCmdNode[] = "cmd";
static const char g_szParamNode[] = "param";

static const char g_szResponseLightFormat[] = "{ \"light\" : { \"status\" : %u } }";


//--------------------------------------------------------------------------------------------------

request_executor_t::request_executor_t( driver_intf_t& driver )
    : m_driver( driver )
{
}

//--------------------------------------------------------------------------------------------------

request_executor_t::~request_executor_t( )
{
}

//--------------------------------------------------------------------------------------------------

void request_executor_t::execute( const device_command_t& command, device_state_t& state )
{
    m_driver.execute_command( command );
    m_driver.get_state( state );

    if ( command.cmd != 0 )
    {
        LOG_TRACE( "Command retrieved: %u, param: %u. Result state = %u",
            command.cmd, command.param, state.lights );
    }
}

//--------------------------------------------------------------------------------------------------
