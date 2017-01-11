#pragma once

#include <memory>


struct device_command_t;


namespace smarty
{

class command_t;

//--------------------------------------------------------------------------------------------------

class command_factory_t
{
public:
    virtual ~command_factory_t( ) { }

    virtual std::shared_ptr< command_t >
    create_device_command( const device_command_t& cmd, uint timeout ) = 0;
};

//--------------------------------------------------------------------------------------------------

} // namespace smarty
