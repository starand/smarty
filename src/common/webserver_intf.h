#ifndef __INTERFACES_H_INCLUDED
#define __INTERFACES_H_INCLUDED

#include <common/errors.h>
#include <memory>


class config_t;
class driver_intf_t;

class web_server_intf_t
{
public:
    virtual ~web_server_intf_t( ) { }

    virtual ErrorCode start( std::shared_ptr< driver_intf_t > driver,
                             std::shared_ptr< config_t > config ) = 0;
    virtual ErrorCode stop( ) = 0;
};

#endif // __INTERFACES_H_INCLUDED
