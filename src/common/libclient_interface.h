#ifndef __LIBCLIENT_INTERFACE_H_INCLUDED
#define __LIBCLIENT_INTERFACE_H_INCLUDED

#include <common/errors.h>

#ifdef WINDOWS
#define DLLMODE __declspec(dllexport)
#else
#	define DLLMODE extern
#endif


struct mobile_notification_t;
struct desktop_command_request_t;
struct server_command_request_t;

typedef void( *change_state_callback_f )( uchar state );
typedef void( *notification_callback_f )( mobile_notification_t *notification );


extern "C"
{
    DLLMODE bool initialize( const char *host, ushort port, const char *password,
                             change_state_callback_f change_state_callback,
                             notification_callback_f notification_callback );
    DLLMODE void finalize( );

    DLLMODE void turn_light( uchar command, uchar param );
    DLLMODE void desktop_command( const desktop_command_request_t& command );
    DLLMODE ErrorCode server_command( const server_command_request_t& cmd );

    DLLMODE const char *get_last_error( );
    DLLMODE const char *get_config( );

    DLLMODE void set_loggers( const char *source, void *loggers );
};


typedef bool( *initialize_funct_t )( const char *host, ushort port, const char *password,
                                     change_state_callback_f change_state_callback,
                                     notification_callback_f notification_callback );
typedef void( *finalize_funct_t )( );
typedef bool( *turn_light_funct_t )( uchar command, uchar param );
typedef void( *desktop_command_funct_t )( const desktop_command_request_t& command );
typedef ErrorCode( *server_command_funct_t )( const server_command_request_t& command );

typedef const char *( *get_last_error_funct_t )( );
typedef const char *( *get_config_funct_t )( );
typedef bool( *set_loggers_funct_t )( const char *source, void *loggers );

#endif // __LIBCLIENT_INTERFACE_H_INCLUDED
