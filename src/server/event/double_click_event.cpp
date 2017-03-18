#include <common/StdAfx.h>

#include <event/double_click_event.h>


//--------------------------------------------------------------------------------------------------

double_click_event_t::double_click_event_t( uint pin, uint mode, const uint& last_dblclick,
                                            smarty::command_handler_t& command_handler )
    : event_t( mode )
    , m_pin( pin )
    , m_last_dblclicked( last_dblclick )
    , m_command_handler( command_handler )
{
}

//--------------------------------------------------------------------------------------------------

double_click_event_t::~double_click_event_t( )
{
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void double_click_event_t::on_event( )
{
    if ( m_last_dblclicked != m_pin )
    {
        return;
    }

    for ( auto& action : m_actions )
    {
        m_command_handler.add_command( action );
    }
}

//--------------------------------------------------------------------------------------------------
