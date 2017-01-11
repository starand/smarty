#ifndef __CLIENT_LIB_H_INCLUDED
#define __CLIENT_LIB_H_INCLUDED

#include <common/library_intf.h>
#include <common/libclient_interface.h>
#include <common/errors.h>


class client_lib_t : protected library_intf_t
{
public:
    client_lib_t( );
    virtual ~client_lib_t( );

public:
    bool initialize( change_state_callback_f light_changed_callback,
                     notification_callback_f notification_callback );

private:
    virtual bool load_functions( );

public:
    bool connect( const char *host, ushort port, const char* password, string& error );
    void disconnect( );

    bool is_connected( ) const;

    bool turn_light( uchar command, uchar param );
    void desktop_command( const desktop_command_request_t& command );
    ErrorCode server_command( const server_command_request_t& cmd );

    const char *get_config( ) const;

private:
    friend void light_change_callback( uchar state );
    friend void notification_callback( mobile_notification_t *notification );

    void on_light_changed( uchar state );
    void on_notification( mobile_notification_t *notification );

private:
    initialize_funct_t	m_initialize;
    finalize_funct_t	m_finalize;
    turn_light_funct_t	m_turn_light;
    desktop_command_funct_t m_desktop_command;
    server_command_funct_t  m_server_command;
    get_last_error_funct_t m_get_last_error;
    get_config_funct_t m_get_config;
    set_loggers_funct_t m_set_loggers;

    change_state_callback_f m_light_changed_callback;
    notification_callback_f m_notification_callback;

    volatile bool m_connected;
};

#endif // __CLIENT_LIB_H_INCLUDED
