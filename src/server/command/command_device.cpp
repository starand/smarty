#include <common/StdAfx.h>

#include <command/command_device.h>
#include <device/device.h>
#include <device/light_object.h>
#include <event/event_parser.h>


//--------------------------------------------------------------------------------------------------

command_device_t::command_device_t( const device_command_t& cmd, uint timeout,
                                    light_object_t& light_object, device_t& device )
    : m_cmd( cmd )
    , m_timeout( timeout )
    , m_light_object( light_object )
    , m_device( device )
{
    LOG_TRACE( "[cmd.%p] created: %u, param: %u, timeout = %u",
               this, m_cmd.cmd, m_cmd.param, m_timeout );
}

//--------------------------------------------------------------------------------------------------

ErrorCode command_device_t::execute( )
{
    LOG_TRACE( "[cmd.%p] cmd: %u, param: %u, timeout = %u",
               this, m_cmd.cmd, m_cmd.param, m_timeout );

    if ( m_timeout )
    {
        time_t exec_time = time( nullptr ) + m_timeout;
        m_light_object.set_turnoff_time( exec_time );

        return ErrorCode::OK;
    }

    auto code = m_device.execute_command( m_cmd );
    LOG_TRACE( "[cmd.%p] Command device [%u:%u] completed with code %u",
               this, m_cmd.cmd, m_cmd.param, code );

    return code;
}

//--------------------------------------------------------------------------------------------------
