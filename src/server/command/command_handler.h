#pragma once

#include <memory>


namespace smarty
{

class command_t;

//--------------------------------------------------------------------------------------------------

class command_handler_t
{
public:
    virtual ~command_handler_t( ) { }

    virtual void add_command( std::shared_ptr< command_t > cmd ) = 0;
};

//--------------------------------------------------------------------------------------------------

} // namespace smarty
