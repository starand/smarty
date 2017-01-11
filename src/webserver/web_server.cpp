#include <common/StdAfx.h>
#include "web_server.h"

#include "thread_pool.h"

#include <xsocket.h>


#define DEFAULT_HTTP_PORT		80

//--------------------------------------------------------------------------------------------------

web_server_t::web_server_t( )
    : m_socket( NULL )
    , m_clients_queue( )
    , m_thread_pool( NULL )
{
}

//--------------------------------------------------------------------------------------------------

web_server_t::~web_server_t( )
{
}

//--------------------------------------------------------------------------------------------------

void web_server_t::create_internal_objects( std::shared_ptr< driver_intf_t > driver,
                                            std::shared_ptr< smarty_config_t > config )
{
    ASSERT( m_socket == NULL );
    m_socket = new socket_t( );

    ASSERT( m_thread_pool == NULL );
    m_thread_pool = new thread_pool_t( m_clients_queue, driver, config );
}

//--------------------------------------------------------------------------------------------------

void web_server_t::destroy_internal_objects( )
{
    FREE_POINTER( m_socket );
    FREE_POINTER( m_thread_pool );
}

//--------------------------------------------------------------------------------------------------

/*virtual */
ErrorCode web_server_t::start( std::shared_ptr< driver_intf_t > driver,
                               std::shared_ptr< smarty_config_t > config )
{
    create_internal_objects( driver, config );
    ASSERT( m_thread_pool != NULL ); ASSERT( m_socket != NULL );

    ushort port = DEFAULT_HTTP_PORT;
    if ( !m_socket->listen( port ) )
    {
        LOG_ERROR( "Could not bind port %u", port );
        return ErrorCode::OPERATION_FAILED;
    }

    bool started = thread_base_t::start( ); ASSERT( started == true );

    if ( !m_thread_pool->start( 5 ) )
    {
        return ErrorCode::OPERATION_FAILED;
    }

    return ErrorCode::OK;
}

//--------------------------------------------------------------------------------------------------

/*virtual */
ErrorCode web_server_t::stop( )
{
    thread_base_t::stop( );
    thread_base_t::wait( );

    m_thread_pool->stop( );

    destroy_internal_objects( );
    return ErrorCode::OK;
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void web_server_t::do_run( )
{
    while ( true )
    {
        socket_t *client = m_socket->accept( );
        if ( is_stopping( ) )
        {
            delete client;
            break;
        }

        // client is processed by one of pool threads
        m_clients_queue.push( client );
        //LOG_TRACE("Client was pushed to queue");
    }
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void web_server_t::do_stop( )
{
    m_socket->shutdown( );
    m_socket->close( );
}

//--------------------------------------------------------------------------------------------------
