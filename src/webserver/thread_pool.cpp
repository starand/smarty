#include <common/StdAfx.h>
#include "thread_pool.h"

#include "webclient_handler.h"
#include "request_executor.h"

#include <stdlib.h>


#define MIN_POOL_SIZE	1
#define MAX_POOL_SIZE	20


//--------------------------------------------------------------------------------------------------

thread_pool_t::thread_pool_t( clients_queue_t& clients_queue,
                              std::shared_ptr< driver_intf_t > driver,
                              std::shared_ptr< config_t > config )
    : m_clients_queue( clients_queue )
    , m_pool_size( 0 )
    , m_threads( )
    , m_driver( driver )
    , m_config( config )
    , m_request_executor( NULL )
{
    create_internal_objects( );
}

//--------------------------------------------------------------------------------------------------

thread_pool_t::~thread_pool_t( )
{
    destroy_internal_obnjects( );
    ASSERT( m_threads.empty( ) );
}

//--------------------------------------------------------------------------------------------------

void thread_pool_t::create_internal_objects( )
{
    ASSERT( m_request_executor == NULL );
    m_request_executor = new request_executor_t( *m_driver );
}

//--------------------------------------------------------------------------------------------------

void thread_pool_t::destroy_internal_obnjects( )
{
    FREE_POINTER_ASSERT( m_request_executor );
}

//--------------------------------------------------------------------------------------------------

bool thread_pool_t::start( size_t pool_size )
{
    ASSERT( IN_RANGE( pool_size, MIN_POOL_SIZE, MAX_POOL_SIZE ) );
    m_pool_size = pool_size;

    for ( size_t idx = 0; idx < m_pool_size; ++idx )
    {
        webclient_handler_t *handler = new webclient_handler_t( m_clients_queue,
                                                                *m_request_executor, *m_config );

        bool started = handler->start( );
        ASSERT( started == true );
        m_threads.push_back( handler );
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

bool thread_pool_t::stop( )
{
    ASSERT( m_pool_size == m_threads.size( ) );

    // stop and wait should be implemented in different loops
    // because if we did not stop some handler, it can consume
    // stop packets from queue, and current handler in loop will hang
    for ( size_t idx = 0; idx < m_pool_size; ++idx )
    {
        webclient_handler_t *handler = m_threads[ idx ];
        ASSERT( handler != NULL );

        handler->stop( );
        handler->wait( );

        delete handler;
    }

    m_threads.clear( );
    return true;
}

//--------------------------------------------------------------------------------------------------
