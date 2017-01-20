#include <common/StdAfx.h>

#include <client/client_thread.h>
#include <server/net_server.h>

#include <net/xsocket.h>


#define DEFAULT_PORT	53074
#define HANDLERS_COUNT	10


// net_server_t implementation
//--------------------------------------------------------------------------------------------------

net_server_t::net_server_t( smarty::client_handler_factory_t& handler_factory )
    : m_handler_factory( handler_factory )
    , m_socket( new socket_t( ) )
    , m_port( DEFAULT_PORT )
    , m_clients_queue( )
    , m_client_handlers( HANDLERS_COUNT )
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
    if ( !start_client_handlers( ) )
    {
        return false;
    }

    return m_socket->listen( m_port );
}

//--------------------------------------------------------------------------------------------------

void net_server_t::finalize( )
{
    m_socket->close( );

    stop_client_handlers();
    clean_clients_queue( );
}

//--------------------------------------------------------------------------------------------------

void net_server_t::clean_clients_queue( )
{
    while ( !m_clients_queue.empty( ) )
    {
        m_clients_queue.pop( );
    }
}

//--------------------------------------------------------------------------------------------------

bool net_server_t::start_client_handlers( )
{
    for ( auto& handler : m_client_handlers )
    {
        handler.reset( new client_thread_t( m_handler_factory, m_clients_queue ) );
        if ( !handler->start( ) )
        {
            ASSERT_FAIL( "Unable to start client handler" );
        }
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

void net_server_t::stop_client_handlers( )
{
    for ( auto& handler : m_client_handlers )
    {
        ASSERT( handler.get( ) != nullptr );

        handler->stop( );
        handler->wait( );
    }
}

//--------------------------------------------------------------------------------------------------
