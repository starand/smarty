#ifndef __WEBCLIENT_HANDLER_H_INCLUDED
#define __WEBCLIENT_HANDLER_H_INCLUDED

#include <threading.h>


class clients_queue_t;
class socket_t;
class request_parser_t;
class request_executor_t;
class html_builder_t;
class smarty_config_t;


class webclient_handler_t : public thread_base_t
{
public:
    webclient_handler_t( clients_queue_t& clients_queue, request_executor_t& request_executor,
                         smarty_config_t& config );
    ~webclient_handler_t( );

private:
    void create_internal_objects( );
    void allocate_buffer( );

    void destroy_internal_objects( );
    void free_buffer( );

public:
    virtual void do_run( );
    virtual void do_stop( );

private:
    void process_client( );

    bool process_favicon_request( );
    bool process_jquery_request( );
    bool process_command_request( std::string& request );
    bool process_index_request( );
    void process_invalid_request( );

    bool send_response( const void *data, size_t len ) const;
    bool execute_request( const std::string& request, std::string& response );
private:
    clients_queue_t& m_clients_queue;

    char *m_buffer;
    size_t m_buffer_size;

    request_parser_t *m_request_parser;
    request_executor_t& m_request_executor;
    html_builder_t *m_html_builder;
    smarty_config_t& m_config;

private: // current client info
    socket_t *m_socket;
    std::string m_endpoint;

};

#endif // __WEBCLIENT_HANDLER_H_INCLUDED
