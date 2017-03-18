#pragma once

#include <memory>
#include <vector>


namespace smarty
{

class command_t;

//--------------------------------------------------------------------------------------------------

class event_t
{
public:
    event_t( uint mode ) : m_mode( mode ) { }
    virtual ~event_t( ) { }

    virtual void on_event( ) = 0;

    uint get_mode( ) const
    {
        return m_mode;
    }

    void set_actions( std::vector< std::shared_ptr< smarty::command_t > >& actions )
    {
        m_actions.swap( actions );
    }

protected:
    uint m_mode;

    std::vector< std::shared_ptr< smarty::command_t > > m_actions;
};

//--------------------------------------------------------------------------------------------------

} // smarty namespace
