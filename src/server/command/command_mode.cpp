#include <common/StdAfx.h>

#include <command/command_mode.h>
#include <event/event_handler.h>


//--------------------------------------------------------------------------------------------------

command_mode_t::command_mode_t( mode_cmd_t cmd, uint delay, event_handler_t& event_handler )
    : m_cmd( cmd )
    , m_delay( delay )
    , m_event_handler( event_handler )
{
    LOG_TRACE( "[cmd.%p] Mode command created: mode bit %u, onOff: %s, delay = %u",
               this, m_cmd.mode_bit, ( m_cmd.onOff ? "on" : "off" ), m_delay );
}

//--------------------------------------------------------------------------------------------------

ErrorCode command_mode_t::execute( )
{
    m_event_handler.set_mode_bit( m_cmd.mode_bit, m_cmd.onOff );
    return ErrorCode::OK;
}

//--------------------------------------------------------------------------------------------------
