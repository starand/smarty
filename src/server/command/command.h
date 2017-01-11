#pragma once

#include <common/errors.h>


class driver_intf_t;

namespace smarty
{

//--------------------------------------------------------------------------------------------------

class command_t
{
public:
    virtual ~command_t( ) { }

    virtual ErrorCode execute( driver_intf_t& driver ) = 0;
};

//--------------------------------------------------------------------------------------------------

} // namespace smarty
