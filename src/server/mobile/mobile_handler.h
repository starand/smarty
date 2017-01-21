#ifndef __MOBILE_HANDLER_H_INCLUDED
#define __MOBILE_HANDLER_H_INCLUDED

#include <client/client_handler.h>


namespace smarty
{
    class client_register_t;
}
class device_t;
class event_handler_t;
class mobile_register_t;
class packet_intf_t;
class smarty_config_t;
class socket_t;
struct mobile_handshake_request_t;
struct server_command_request_t;

class mobile_handler_t : public smarty::client_handler_t
{
public:
    mobile_handler_t( socket_t& socket, const std::string& endpoint, const smarty_config_t& config,
                      device_t& driver, mobile_register_t& mobile_register,
                      smarty::client_register_t& clients, packet_intf_t *hs_req,
                      event_handler_t& event_handler );
    ~mobile_handler_t( );

public:
    virtual bool make_handshake( );
    virtual void process_client( );

private:
    bool update_mobile_config( );
    bool send_config_update_response( );

    void add_server_command( const server_command_request_t& request );
    void add_command_delay( const std::string& params );
    void add_command_light( const std::string& params );

private:
    socket_t& m_socket;
    const std::string m_end_point;

    mobile_handshake_request_t *m_hs_req;

    const smarty_config_t& m_config;

    device_t& m_device;
    mobile_register_t& m_mobile_register;
    smarty::client_register_t& m_clients;

    event_handler_t& m_event_handler;
};

#endif // __MOBILE_HANDLER_H_INCLUDED
