#ifndef __COMMAND_SENDER_H_INCLUDED
#define __COMMAND_SENDER_H_INCLUDED

#include "smarty_connector.h"

#include <threading.h>


class socket_t;

class command_sender_t : public thread_base_t
{
public:
    command_sender_t( smarty_connector_intf_t& connector, socket_t& socket,
                      send_queue_t& send_queue );
    ~command_sender_t( );

public:
    virtual void do_run( );
    virtual void do_stop( );

private:
    smarty_connector_intf_t& m_connector;
    socket_t& m_socket;
    send_queue_t& m_send_queue;
};

#endif // __COMMAND_SENDER_H_INCLUDED
