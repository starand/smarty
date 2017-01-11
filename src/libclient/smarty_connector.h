#ifndef __SMARTY_CONNECTOR_H_INCLUDED
#define __SMARTY_CONNECTOR_H_INCLUDED

#include <common/client_protocol.h>
#include <common/errors.h>
#include <common/libclient_interface.h>

#include <lock_queue.h>
#include <threading.h>


//typedef lock_queue_t<packet_intf_t *> send_queue_t;
class send_queue_t : public lock_queue_t < packet_intf_t * >
{
};

class smarty_connector_intf_t
{
public:
    virtual void reconnect( ) = 0;
    virtual void wait_connected( ) = 0;
};


class socket_t;
class command_sender_t;
class notification_listener_t;


class smarty_connector_t : public thread_base_t, public smarty_connector_intf_t
{
public:
    smarty_connector_t( change_state_callback_f change_state_callback,
                        notification_callback_f notification_callback );
    ~smarty_connector_t( );

private:
    void create_internal_objects( );
    void destroy_internal_objects( );

public:
    virtual void do_run( );
    virtual void do_stop( );

public:
    bool connect( const char *host, ushort port, const char *password );
    void disconnect( );

    void reconnect( );
    void wait_connected( );

    void turn_light( uchar command, uchar param );
    void desktop_command( const desktop_command_request_t& command );
    ErrorCode server_command( const server_command_request_t& cmd );

    const char *get_last_error( ) const;
    const char *get_config( ) const;

private:
    bool do_connect( );
    bool make_handshake( );
    bool check_handshake_response( mobile_handshake_response_t& response );

    void connection_finalize( );

    bool update_config( );
    void reconnected_notification( );

private:
    socket_t *m_socket;
    change_state_callback_f m_change_state_callback;
    notification_callback_f m_notification_callback;

    send_queue_t m_send_queue;

    command_sender_t *m_command_sender;
    notification_listener_t *m_notification_listener;

    event_t m_connection_start_event;
    event_t m_connection_finished_event;
    event_t m_connection_established;
    bool m_connected;

    string m_host;
    ushort m_port;
    string m_password;

    string m_last_error;
    string m_config;
};

#endif // __SMARTY_CONNECTOR_H_INCLUDED
