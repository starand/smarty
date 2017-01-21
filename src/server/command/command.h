#pragma once

#include <common/errors.h>


namespace smarty
{

//--------------------------------------------------------------------------------------------------

class command_t
{
public:
    virtual ~command_t( ) { }

    virtual ErrorCode execute( ) = 0;
};

//--------------------------------------------------------------------------------------------------

} // namespace smarty
