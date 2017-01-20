#pragma once

#include <common/errors.h>


class device_t;

namespace smarty
{

//--------------------------------------------------------------------------------------------------

class command_t
{
public:
    virtual ~command_t( ) { }

    virtual ErrorCode execute( device_t& device ) = 0;
};

//--------------------------------------------------------------------------------------------------

} // namespace smarty
