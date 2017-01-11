#include <common/StdAfx.h>

#include <client/client_queue.h>
#include <server/net_server.h>

#include <net/xsocket.h>


#define DEFAULT_PORT	53074


//--------------------------------------------------------------------------------------------------

net_server_t::net_server_t( clients_queue_t& clients_queue )
    : m_socket( new socket_t( ) )
    , m_port( DEFAULT_PORT )
    , m_clients_queue( clients_queue )
{
    ASSERT( m_socket.get( ) );
}


//--------------------------------------------------------------------------------------------------

/*virtual */
void net_server_t::do_run( )
{
    if ( !init( ) )
    {
        return;
    }

    LOG_INFO( "[server] Listening on port %u", m_port );

    while ( true )
    {
        socket_t *client = m_socket->accept( );
        if ( is_stopping( ) )
        {
            break;
        }

        ASSERT( client != nullptr );
        m_clients_queue.push( client );
    }

    finalize( );
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void net_server_t::do_stop( )
{
    m_socket->shutdown( SD_BOTH );
    m_socket->close( );
}

//--------------------------------------------------------------------------------------------------

bool net_server_t::init( )
{
    return m_socket->listen( m_port );
}

//--------------------------------------------------------------------------------------------------

void net_server_t::finalize( )
{
    m_socket->close( );
}

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
