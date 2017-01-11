#ifndef __MOBILE_HANDLER_H_INCLUDED
#define __MOBILE_HANDLER_H_INCLUDED

#include <client/client_handler.h>


class socket_t;
class smarty_config_t;
class device_controller_t;
class event_handler_t;
class mobile_register_t;
class smarty_server_t;
class command_processor_t;
struct mobile_handshake_request_t;
struct server_command_request_t;
class packet_intf_t;


class mobile_handler_t : public smarty::client_handler_t
{
public:
    mobile_handler_t( socket_t& socket, const std::string& endpoint, smarty_config_t& config,
                      device_controller_t& driver, mobile_register_t& mobile_register,
                      smarty_server_t& smarty_server, packet_intf_t *hs_req,
                      command_processor_t& command_handler, event_handler_t& event_handler );
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

    smarty_config_t& m_config;

    device_controller_t& m_device;
    mobile_register_t& m_mobile_register;
    smarty_server_t& m_smarty_server;

    command_processor_t& m_command_handler;
    event_handler_t& m_event_handler;
};

#endif // __MOBILE_HANDLER_H_INCLUDED
