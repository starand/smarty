#ifndef __CLIENT_PROTOCOL_H_INCLUDED
#define __CLIENT_PROTOCOL_H_INCLUDED

#include <common/enums.h>

#include <driver/driver.h>

#include <platform.h>
#include <asserts.h>
#include <struct_serializer_ms.h>

#include <memory>

#ifdef WINDOWS
#   define sprintf sprintf_s
#endif


static const char g_szUnableRecvDataFromSocket[] = "Could not receive data from socket";
static const char g_szInvalidPacketType[] = "Invalid packet type %u";


#define RECV_PACKET_SWITCH(_SOCKET_) \
	socket_t *_socket = _SOCKET_; \
	char *_error = NULL; \
	EPACKETMAGIC packet_magic = EPM_ERROR; \
	if (_socket->recv(&packet_magic, sizeof(EPACKETMAGIC)) != sizeof(EPACKETMAGIC)) \
    { _error = (char*)g_szUnableRecvDataFromSocket; packet_magic = EPM_ERROR; } \
    else { \
	switch(packet_magic) { {

#define RECV_PACKET_CASE(_PACKET_TYPE_, _PACKET_) \
	break; } \
	case _PACKET_TYPE_::m_packet_magic: { \
	std::unique_ptr< _PACKET_TYPE_ > _packet_ptr(new _PACKET_TYPE_()); \
	_PACKET_TYPE_& _PACKET_ = *_packet_ptr; \
	struct_serializer_t<_PACKET_TYPE_>::serializer_t serializer; \
    (void)serializer; \
	if (!serializer.recv(*_socket, _PACKET_)) \
    { _error = (char*)g_szUnableRecvDataFromSocket; packet_magic = EPM_ERROR; } \
    else

#define RECV_PACKET_CASE_EXISTS(_PACKET_TYPE_, _PACKET_) \
	break; } \
	case _PACKET_TYPE_::m_packet_magic: { \
	typename struct_serializer_t<_PACKET_TYPE_>::serializer_t serializer; \
    (void)serializer; \
	if (!serializer.recv(*_socket, _PACKET_)) \
    { _error = (char*)g_szUnableRecvDataFromSocket; packet_magic = EPM_ERROR; } \
    else

#define RECV_PACKET_ERROR(_ERROR_) \
	break; } \
	default: \
    static char g_szErrorMsgBuffer[ 1024 ] = { 0 }; \
	sprintf(g_szErrorMsgBuffer, g_szInvalidPacketType, packet_magic); \
	_error = (char*)g_szErrorMsgBuffer; packet_magic = EPM_INVALIDMAGIC; \
	} /*switch*/ \
	} /*if*/ \
	const char *_ERROR_ = _error; \
	if (_ERROR_)

/*
Usage example :

socket_t *socket ... ;

RECV_PACKET_SWITCH(socket)
RECV_PACKET_CASE(mobile_handshake_request_t, request)
{
// TODO : process request
break;
}
RECV_PACKET_CASE(mobile_handshake_request_t, request)
{
// TODO : process request
break;
}
RECV_PACKET_ERROR()
{
// TODO : process error
}

*/

//--------------------------------------------------------------------------------------------------

class packet_intf_t
{
public:
    virtual bool send( socket_t& socket ) = 0;
    virtual bool recv( socket_t& socket ) = 0;

    virtual ~packet_intf_t( )
    {
    }
};

//--------------------------------------------------------------------------------------------------

template <typename type_>
struct packet_base_t : public packet_intf_t
{
    virtual ~packet_base_t( )
    {
    }

    bool send( socket_t& socket )
    {
        type_ *packet = static_cast<type_ *>( this );
        ASSERT( packet != NULL );

        EPACKETMAGIC packet_magic = type_::m_packet_magic;
        if ( socket.send(& packet_magic, sizeof( EPACKETMAGIC ) ) != sizeof( EPACKETMAGIC ) )
        {
            return false;
        }

        //LOG_TRACE("[protocol] Packet magic: %u", (uint)packet_magic);

        typename struct_serializer_t<type_>::serializer_t serializer;
        if ( !serializer.send( socket, *packet ) )
        {
            return false;
        }

        return true;
    }

    bool recv( socket_t& socket )
    {
        EPACKETMAGIC packet_magic;
        if ( socket.recv(& packet_magic, sizeof( packet_magic ) ) != sizeof( packet_magic ) )
        {
            return false;
        }

        if ( packet_magic != type_::m_packet_magic )
        {
            return false;
        }

        type_ *packet = static_cast<type_ *>( this );
        ASSERT( packet != NULL );

        typename struct_serializer_t<type_>::serializer_t serializer;
        if ( !serializer.recv( socket, *packet ) )
        {
            return false;
        }

        return true;
    }
};


//--------------------------------------------------------------------------------------------------

struct mobile_handshake_request_t : public packet_base_t < mobile_handshake_request_t >
{
    static const EPACKETMAGIC m_packet_magic = EPM_MOBILE_HANDSHAKE_REQUEST;

    std::string password;
    uint version;
};

STRUCT_SERIALIZER_BEGIN( mobile_handshake_request_t )
STRUCT_SERIALIZER_FIELD( std::string, password )
STRUCT_SERIALIZER_FIELD( uint, version )
STRUCT_SERIALIZER_END( )

//--------------------------------------------------------------------------------------------------

struct mobile_handshake_response_t : public packet_base_t < mobile_handshake_response_t >
{
    static const EPACKETMAGIC m_packet_magic = EPM_MOBILE_HANDSHAKE_RESPONSE;

    ESTATE state;
    uchar light_state;
    uint heartbeat_timeout;
    uint event_modes;
};

STRUCT_SERIALIZER_BEGIN( mobile_handshake_response_t )
STRUCT_SERIALIZER_FIELD( ESTATE, state )
STRUCT_SERIALIZER_FIELD( uchar, light_state )
STRUCT_SERIALIZER_FIELD( uint, event_modes )
STRUCT_SERIALIZER_END( )

//--------------------------------------------------------------------------------------------------

struct state_request_t : public packet_base_t < state_request_t >
{
    static const EPACKETMAGIC m_packet_magic = EPM_STATE_REQUEST;
};

STRUCT_SERIALIZER_BEGIN( state_request_t )
STRUCT_SERIALIZER_END( )

//--------------------------------------------------------------------------------------------------

struct command_request_t : public packet_base_t < command_request_t >
{
    static const EPACKETMAGIC m_packet_magic = EPM_COMMAND_REQUEST;

    uchar command;
    uchar param;
};

STRUCT_SERIALIZER_BEGIN( command_request_t )
STRUCT_SERIALIZER_FIELD( uchar, command )
STRUCT_SERIALIZER_FIELD( uchar, param )
STRUCT_SERIALIZER_END( )

//--------------------------------------------------------------------------------------------------

struct disconnect_request_t : public packet_base_t < disconnect_request_t >
{
    static const EPACKETMAGIC m_packet_magic = EPM_COMMAND_DISCONNECT;
};

STRUCT_SERIALIZER_BEGIN( disconnect_request_t )
STRUCT_SERIALIZER_END( )

//--------------------------------------------------------------------------------------------------

struct light_notification_t : public packet_base_t < light_notification_t >
{
    static const EPACKETMAGIC m_packet_magic = EPM_STATE_CHANGE_NOTIFICATION;

    lights_state_t lights_state;
};

STRUCT_SERIALIZER_BEGIN( light_notification_t )
STRUCT_SERIALIZER_FIELD( lights_state_t, lights_state )
STRUCT_SERIALIZER_END( )


//--------------------------------------------------------------------------------------------------

struct mobile_notification_t : public packet_base_t < mobile_notification_t >
{
    static const EPACKETMAGIC m_packet_magic = EPM_MOBILE_NOTIFICATION;

    uint desktop_index;
    EMOBILENOTIFICATION type;
    std::string params;
};

STRUCT_SERIALIZER_BEGIN( mobile_notification_t )
STRUCT_SERIALIZER_FIELD( uint, desktop_index )
STRUCT_SERIALIZER_FIELD( EMOBILENOTIFICATION, type )
STRUCT_SERIALIZER_FIELD( std::string, params )
STRUCT_SERIALIZER_END( )


//--------------------------------------------------------------------------------------------------

struct mobile_heartbeat_request_t : public packet_base_t < mobile_heartbeat_request_t >
{
    static const EPACKETMAGIC m_packet_magic = EPM_MOBILE_HEARTBEAT_REQUEST;
};

STRUCT_SERIALIZER_BEGIN( mobile_heartbeat_request_t )
STRUCT_SERIALIZER_END( )


//--------------------------------------------------------------------------------------------------

struct mobile_heartbeat_response_t : public packet_base_t < mobile_heartbeat_response_t >
{
    static const EPACKETMAGIC m_packet_magic = EPM_MOBILE_HEARTBEAT_RESPONSE;
};

STRUCT_SERIALIZER_BEGIN( mobile_heartbeat_response_t )
STRUCT_SERIALIZER_END( )


//--------------------------------------------------------------------------------------------------

struct config_update_request_t : public packet_base_t < config_update_request_t >
{
    static const EPACKETMAGIC m_packet_magic = EPM_CONFIG_UPDATE_REQUEST;
};

STRUCT_SERIALIZER_BEGIN( config_update_request_t )
STRUCT_SERIALIZER_END( )


//--------------------------------------------------------------------------------------------------

struct config_update_response_t : public packet_base_t < config_update_response_t >
{
    static const EPACKETMAGIC m_packet_magic = EPM_CONFIG_UPDATE_RESPONSE;

    std::string config;
};

STRUCT_SERIALIZER_BEGIN( config_update_response_t )
STRUCT_SERIALIZER_FIELD( std::string, config )
STRUCT_SERIALIZER_END( )


//--------------------------------------------------------------------------------------------------

struct desktop_handshake_request_t : public packet_base_t < desktop_handshake_request_t >
{
    static const EPACKETMAGIC m_packet_magic = EPM_DESKTOP_HANDSHAKE_REQUEST;

    std::string password;
    std::string name;
    uint index;
};

STRUCT_SERIALIZER_BEGIN( desktop_handshake_request_t )
STRUCT_SERIALIZER_FIELD( std::string, password )
STRUCT_SERIALIZER_FIELD( std::string, name )
STRUCT_SERIALIZER_FIELD( uint, index )
STRUCT_SERIALIZER_END( )


//--------------------------------------------------------------------------------------------------

struct desktop_handshake_response_t : public packet_base_t < desktop_handshake_response_t >
{
    static const EPACKETMAGIC m_packet_magic = EPM_DESKTOP_HANDSHAKE_RESPONSE;
    ESTATE state;
};

STRUCT_SERIALIZER_BEGIN( desktop_handshake_response_t )
STRUCT_SERIALIZER_FIELD( ESTATE, state )
STRUCT_SERIALIZER_END( )


//--------------------------------------------------------------------------------------------------

struct desktop_command_request_t : public packet_base_t < desktop_command_request_t >
{
    static const EPACKETMAGIC m_packet_magic = EPM_DESKTOP_COMMAND_REQUEST;

    uint desktop_index;
    EDESKTOPCOMMAND command;
    std::string params;
};

STRUCT_SERIALIZER_BEGIN( desktop_command_request_t )
STRUCT_SERIALIZER_FIELD( uint, desktop_index )
STRUCT_SERIALIZER_FIELD( EDESKTOPCOMMAND, command )
STRUCT_SERIALIZER_FIELD( std::string, params )
STRUCT_SERIALIZER_END( )


//--------------------------------------------------------------------------------------------------

struct desktop_heartbeat_request_t : public packet_base_t < desktop_heartbeat_request_t >
{
    static const EPACKETMAGIC m_packet_magic = EPM_DESKTOP_HEARTBEAT_REQUEST;
};

STRUCT_SERIALIZER_BEGIN( desktop_heartbeat_request_t )
STRUCT_SERIALIZER_END( )


//--------------------------------------------------------------------------------------------------

struct desktop_heartbeat_response_t : public packet_base_t < desktop_heartbeat_response_t >
{
    static const EPACKETMAGIC m_packet_magic = EPM_DESKTOP_HEARTBEAT_RESPONSE;

    uint desktop_index;
};

STRUCT_SERIALIZER_BEGIN( desktop_heartbeat_response_t )
STRUCT_SERIALIZER_FIELD( uint, desktop_index )
STRUCT_SERIALIZER_END( )

//--------------------------------------------------------------------------------------------------

struct server_command_request_t : public packet_base_t < server_command_request_t >
{
    static const EPACKETMAGIC m_packet_magic = EPM_SERVER_COMMAND_REQUEST;

    SERVERCOMMAND command;
    std::string params;
};

STRUCT_SERIALIZER_BEGIN( server_command_request_t )
STRUCT_SERIALIZER_FIELD( SERVERCOMMAND, command )
STRUCT_SERIALIZER_FIELD( std::string, params )
STRUCT_SERIALIZER_END( )

//--------------------------------------------------------------------------------------------------

struct update_modes_request_t : public packet_base_t < update_modes_request_t >
{
    static const EPACKETMAGIC m_packet_magic = EPM_UPDATE_MODES_REQUESTS;

    uint state;
};

STRUCT_SERIALIZER_BEGIN( update_modes_request_t )
STRUCT_SERIALIZER_FIELD( uint, state )
STRUCT_SERIALIZER_END( )

//--------------------------------------------------------------------------------------------------

struct modes_update_notification_t : public packet_base_t < modes_update_notification_t >
{
    static const EPACKETMAGIC m_packet_magic = EPM_MODES_UPDTE_NOTIFICATION;

    uint state;
};

STRUCT_SERIALIZER_BEGIN( modes_update_notification_t )
STRUCT_SERIALIZER_FIELD( uint, state )
STRUCT_SERIALIZER_END( )

//--------------------------------------------------------------------------------------------------

#endif // __CLIENT_PROTOCOL_H_INCLUDED
