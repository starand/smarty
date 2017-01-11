#ifndef __DESKTOP_H_INCLUDED
#define __DESKTOP_H_INCLUDED


class config_t;
class smarty_connector_t;
class packet_receiver_t;
class packet_sender_t;

class desktop_t
{
public:
    desktop_t( config_t& config );
    ~desktop_t( );

private:
    void create_internal_objects( );
    void destory_internal_objects( );

public:
    void run( );

private:
    void initialize( );
    void finalize( );

private:
    config_t& m_config;

    smarty_connector_t *m_connector;
    packet_receiver_t *m_packet_receiver;
    packet_sender_t *m_packet_sender;
};

#endif // __DESKTOP_H_INCLUDED
