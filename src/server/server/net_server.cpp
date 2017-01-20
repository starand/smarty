#include <common/StdAfx.h>

#include <client/client_queue.h>
#include <server/net_server.h>

#include <net/xsocket.h>


#define DEFAULT_PORT	53074


//--------------------------------------------------------------------------------------------------

net_server_t::net_server_t( )
    : m_socket( new socket_t( ) )
    , m_port( DEFAULT_PORT )
    , m_clients_queue( )
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

clients_queue_t& net_server_t::get_client_queue( ) const
{
    return const_cast< clients_queue_t& >( m_clients_queue );
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
    clean_clients_queue( );
}

//--------------------------------------------------------------------------------------------------

void net_server_t::clean_clients_queue( )
{
    while ( !m_clients_queue.empty( ) )
    {
        socket_t* client = m_clients_queue.pop( );
        FREE_POINTER( client );
    }
}

//--------------------------------------------------------------------------------------------------
