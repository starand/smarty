#include <common/StdAfx.h>

#include <client/client_thread.h>
#include <client/client_handler.h>
#include <server/smarty_server.h>

#include <common/client_protocol.h>

#include <xsocket.h>
#include <strutils.h>
#include <scoped_ptr.h>

#ifdef LINUX
#	include <unistd.h>
#endif


extern const char g_szUnableToSendResponse[] = "Could not send %s response to client %s";


// client_thread_t implementation
//--------------------------------------------------------------------------------------------------

client_thread_t::client_thread_t( smarty::client_handler_factory_t& clients_factory,
                                  clients_queue_t& clients_queue )
    : m_end_point( )
    , m_socket( NULL )
    , m_clients_queue( clients_queue )
    , m_clients_factory( clients_factory )
{
}

//--------------------------------------------------------------------------------------------------

client_thread_t::~client_thread_t( )
{
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void client_thread_t::do_run( )
{
    while ( true )
    {
        scoped_ptr_t< socket_t > client( m_clients_queue.pop( ) );
        if ( is_stopping( ) )
        {
            break;
        }

        StrUtils::FormatString( m_end_point, "%s:%u",
                                client->get_remote_address( ), client->get_remote_port( ) );
        m_socket = client.get( );

        smarty::client_handler_t *client_handler = create_client_handler( );

        if ( client_handler == NULL )
        {
            finish_connection( );
            continue;
        }

        scoped_ptr_t< smarty::client_handler_t > handler( client_handler );
        if ( !handler->make_handshake( ) )
        {
            finish_connection( );
            continue;
        }

        handler->process_client( );

        finish_connection( );
    }
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void client_thread_t::do_stop( )
{
    socket_t *client = new socket_t( );
    m_clients_queue.push( client );
}

//--------------------------------------------------------------------------------------------------

smarty::client_handler_t *client_thread_t::create_client_handler( )
{
    ASSERT( m_socket != NULL );

    RECV_PACKET_SWITCH( m_socket );
    RECV_PACKET_CASE( mobile_handshake_request_t, request )
    {
        return m_clients_factory.create_mobile_handler( *m_socket, m_end_point.c_str( ), &request );
    }
    RECV_PACKET_CASE( desktop_handshake_request_t, request )
    {
        return m_clients_factory.create_desktop_handler( *m_socket, m_end_point.c_str( ), &request );
    }
    RECV_PACKET_ERROR( message )
    {
        LOG_ERROR( "[client] Error : %s from %s", message, m_end_point.c_str( ) );
        return NULL;
    }

    return NULL;
}

//--------------------------------------------------------------------------------------------------

void client_thread_t::finish_connection( )
{
    ASSERT( m_socket != NULL );
    m_socket->close( );
}

//--------------------------------------------------------------------------------------------------
