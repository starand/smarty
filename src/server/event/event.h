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
    virtual ~event_t( ) { }

    virtual void on_event( ) = 0;

    virtual uint get_mode( ) const = 0;
    virtual void set_actions( std::vector< std::shared_ptr< smarty::command_t > >& actions ) = 0;
};

//--------------------------------------------------------------------------------------------------

} // smarty namespace
