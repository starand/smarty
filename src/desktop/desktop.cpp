#include <common/StdAfx.h>
#include "desktop.h"

#include "smarty_connector.h"
#include "packet_receiver.h"
#include "packet_sender.h"

#include <utils.h>


#define RECONENCT_TIMEOUT		10000


// desktop_t implementation
//--------------------------------------------------------------------------------------------------

desktop_t::desktop_t( config_t& config )
    : m_config( config )
    , m_connector( NULL )
    , m_packet_receiver( NULL )
    , m_packet_sender( NULL )
{
    create_internal_objects( );
}

//--------------------------------------------------------------------------------------------------

desktop_t::~desktop_t( )
{
    destory_internal_objects( );
}

//--------------------------------------------------------------------------------------------------

void desktop_t::create_internal_objects( )
{
    ASSERT( m_connector == NULL );
    m_connector = new smarty_connector_t( m_config );

    ASSERT( m_packet_sender == NULL );
    m_packet_sender = new packet_sender_t( );

    ASSERT( m_packet_receiver == NULL );
    m_packet_receiver = new packet_receiver_t( *m_packet_sender, m_config );
}

//--------------------------------------------------------------------------------------------------

void desktop_t::destory_internal_objects( )
{
    FREE_POINTER_ASSERT( m_packet_receiver );
    FREE_POINTER_ASSERT( m_packet_sender );
    FREE_POINTER_ASSERT( m_connector );
}

//--------------------------------------------------------------------------------------------------

void desktop_t::run( )
{
    ASSERT( m_connector != NULL );
    initialize( );

    do
    {
        while ( !m_connector->connect( ) )
        {
            LOG_WARNING( "Could not connect to smarty server. Try again .." );
            utils::sleep_ms( RECONENCT_TIMEOUT );
        }

        m_packet_sender->set_socket( m_connector->get_socket( ) );
        m_packet_receiver->set_desktop_index( m_connector->get_hash( ) );
        m_packet_receiver->process_commands( );

        m_packet_sender->set_socket( NULL );
        m_connector->finish_connection( );
    }
    while ( true );

    finalize( );
}

//--------------------------------------------------------------------------------------------------

void desktop_t::initialize( )
{
    ASSERT( m_connector != NULL );
    bool result = m_connector->start( );
    ASSERT( result == true );

    ASSERT( m_packet_sender != NULL );
    result = m_packet_sender->start( );
    ASSERT( result == true );
}

//--------------------------------------------------------------------------------------------------

void desktop_t::finalize( )
{
    ASSERT( m_connector != NULL );
    m_connector->stop( );
    m_connector->wait( );

    ASSERT( m_packet_sender != NULL );
    m_packet_sender->stop( );
    m_packet_sender->wait( );
}

//--------------------------------------------------------------------------------------------------
