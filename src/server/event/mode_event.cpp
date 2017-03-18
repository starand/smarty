#include <common/StdAfx.h>

#include <event/mode_event.h>


//--------------------------------------------------------------------------------------------------

mode_event_t::mode_event_t( uint mode, bool enabled,
                            smarty::command_handler_t& command_handler,
                            const uint& modes_bitset )
    : event_t( mode )
    , m_is_enabled( enabled )
    , m_modes_bitset( modes_bitset )
    , m_prev_bitset( modes_bitset )
    , m_command_handler( command_handler )
{
    LOG_TRACE("[event.%p] created - type: mode, id: %u, fire_on: %u ",
              this, mode, (uint)m_is_enabled );
}

//--------------------------------------------------------------------------------------------------

mode_event_t::~mode_event_t( )
{
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void mode_event_t::on_event( )
{
    uint mode_bit = 1 << ( m_mode - 1);
    bool fired = ( m_modes_bitset ^ m_prev_bitset ) & mode_bit;
    m_prev_bitset = m_modes_bitset;

    if ( !fired )
    {
        return;
    }

    LOG_TRACE("[event.%p] fired, current bitset: %u", this, m_modes_bitset);

    bool current_state = ( m_modes_bitset & mode_bit ) != 0;
    if ( current_state != m_is_enabled )
    {
        return;
    }

    for ( auto& action : m_actions )
    {
        m_command_handler.add_command( action );
    }
}

//--------------------------------------------------------------------------------------------------
