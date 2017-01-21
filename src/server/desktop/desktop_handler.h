#ifndef __DESKTOP_HANDLER_H_INCLUDED
#define __DESKTOP_HANDLER_H_INCLUDED

#include <client/client_handler.h>


class socket_t;
class smarty_config_t;
class desktop_register_t;
struct desktop_handshake_request_t;
class packet_intf_t;

namespace smarty
{
    class client_register_t;
}


class desktop_handler_t : public smarty::client_handler_t
{
public:
    desktop_handler_t( socket_t& socket, const std::string& endpoint, const smarty_config_t& config,
                       desktop_register_t& desktop_register, smarty::client_register_t& clients,
                       packet_intf_t *hs_req );
    ~desktop_handler_t( );

public:
    virtual bool make_handshake( );
    virtual void process_client( );

private:
    std::string m_client_name;
    const std::string m_end_point;
    uint m_desktop_index;

    desktop_handshake_request_t *m_hs_req;

    socket_t& m_socket;
    const smarty_config_t& m_config;

    desktop_register_t& m_desktop_register;
    smarty::client_register_t& m_clients;
};

#endif // __DESKTOP_HANDLER_H_INCLUDED
