#ifndef __SMARTY_CONNECTOR_H_INCLUDED
#define __SMARTY_CONNECTOR_H_INCLUDED

#include <threading.h>
#include <xsocket.h>


class config_t;

class smarty_connector_t : public thread_base_t
{
public:
    smarty_connector_t( config_t& config );
    ~smarty_connector_t( );

protected:
    virtual void do_run( );
    virtual void do_stop( );

public:
    bool connect( );
    void finish_connection( );

    socket_t *get_socket( ) const;
    uint get_hash( ) const;

private:
    bool make_handshake( );
    void recalculate_hash( );

private:
    config_t& m_config;

    event_t m_connection_start_event;
    event_t m_connection_established;
    bool m_connected;

    socket_t m_socket;
    uint m_hash;
};

#endif // __SMARTY_CONNECTOR_H_INCLUDED
