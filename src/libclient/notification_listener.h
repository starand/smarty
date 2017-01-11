#ifndef __NOTIFICATION_LISTENER_H_INCLUDED
#define __NOTIFICATION_LISTENER_H_INCLUDED

#include <common/libclient_interface.h>

#include <threading.h>


class socket_t;
class smarty_connector_intf_t;

class notification_listener_t : public thread_base_t
{
public:
    notification_listener_t( smarty_connector_intf_t& connector, socket_t& socket,
                             change_state_callback_f change_state_callback,
                             notification_callback_f notification_callback );
    ~notification_listener_t( );

public:
    virtual void do_run( );
    virtual void do_stop( );

private:
    void process_notification( );

private:
    smarty_connector_intf_t& m_connector;
    socket_t& m_socket;

    change_state_callback_f m_change_state_callback;
    notification_callback_f m_notification_callback;

    uchar m_prev_state;
};

#endif // __NOTIFICATION_LISTENER_H_INCLUDED
