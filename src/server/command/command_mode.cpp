#include <common/StdAfx.h>

#include <command/command_mode.h>
#include <event/event_handler.h>


//--------------------------------------------------------------------------------------------------

command_mode_t::command_mode_t( uint mode_bit, bool onOff, event_handler_t& handler, uint delay )
    : m_event_handler( handler )
    , m_mode_bit( mode_bit )
    , m_onOff( onOff )
    , m_delay( delay )
{
    LOG_TRACE( "[cmd.%p] Mode command created: mode bit %u, onOff: %s, delay = %u",
               this, mode_bit, ( m_onOff ? "on" : "off" ), delay );
}

//--------------------------------------------------------------------------------------------------

ErrorCode command_mode_t::execute( device_t& device )
{
    m_event_handler.set_mode_bit( m_mode_bit, m_onOff );
    return ErrorCode::OK;
}

//--------------------------------------------------------------------------------------------------
