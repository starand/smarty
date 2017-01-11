#pragma once


class socket_t;
class packet_intf_t;


namespace smarty
{

class client_handler_t;

//--------------------------------------------------------------------------------------------------

class client_handler_factory_t
{
public:
    virtual ~client_handler_factory_t( ) { }

    virtual client_handler_t *create_mobile_handler( socket_t& socket, const char *endpoint,
                                                     packet_intf_t *hs_req ) = 0;
    virtual client_handler_t *create_desktop_handler( socket_t& socket, const char *endpoint,
                                                      packet_intf_t *hs_req ) = 0;
};

//--------------------------------------------------------------------------------------------------

}
