#ifndef __PACKET_SENDER_H_INCLUDED
#define __PACKET_SENDER_H_INCLUDED

#include <threading.h>
#include <lock_queue.h>


class socket_t;
class packet_intf_t;

class packet_sender_t : public thread_base_t
{
public:
    packet_sender_t( );
    ~packet_sender_t( );

public:
    virtual void do_run( );
    virtual void do_stop( );

public:
    void set_socket( socket_t *socket );
    socket_t *get_socket( ) const;

    void send_packet( packet_intf_t *packet );

private:
    typedef lock_queue_t<packet_intf_t *> send_queue_t;

private:
    send_queue_t m_send_queue;
    socket_t *m_socket;
};

#endif // __PACKET_SENDER_H_INCLUDED
