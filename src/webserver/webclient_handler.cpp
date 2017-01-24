#include <common/StdAfx.h>
#include "webclient_handler.h"

#include "html_builder.h"
#include "request_executor.h"
#include "request_parser.h"
#include "web_server.h"

#include <common/server_intf.h>
#include <common/driver_intf.h>

#include <scoped_ptr.h>
#include <xsocket.h>
#include <strutils.h>
#include <logger.h>
#include <stdlib.h>


// 10 Kbytes
#define MAX_BUFFER_SIZE	0x2800


const unsigned char g_aszFaviconContent[] = {
#	include "./.aux/favicon.ico.inc"
};

const unsigned char g_aszJQueryContent[] = {
#ifdef LINUX
#	include "./.aux/jquery.js.inc"
#else
    ""
#endif
};

const char g_aszIndexContent[] = {
#	include "./.aux/index.html.inc"
};

//--------------------------------------------------------------------------------------------------

webclient_handler_t::webclient_handler_t( clients_queue_t& clients_queue,
                                          request_executor_t& request_executor,
                                          config_t& config )
    : m_clients_queue( clients_queue )
    , m_buffer( NULL )
    , m_buffer_size( MAX_BUFFER_SIZE )
    , m_request_parser( NULL )
    , m_request_executor( request_executor )
    , m_html_builder( NULL )
    , m_config( config )
    , m_socket( NULL )
    , m_endpoint( )
{
}

//--------------------------------------------------------------------------------------------------

webclient_handler_t::~webclient_handler_t( )
{
}

//--------------------------------------------------------------------------------------------------

void webclient_handler_t::allocate_buffer( )
{
    ASSERT( m_buffer == NULL );
    m_buffer = (char *)malloc( m_buffer_size );
}

//--------------------------------------------------------------------------------------------------

void webclient_handler_t::create_internal_objects( )
{
    allocate_buffer( );

    ASSERT( m_request_parser == NULL );
    m_request_parser = new request_parser_t( );

    ASSERT( m_html_builder == NULL );
    m_html_builder = new html_builder_t( m_config );
}

//--------------------------------------------------------------------------------------------------

void webclient_handler_t::destroy_internal_objects( )
{
    ASSERT( m_html_builder != NULL );
    FREE_POINTER( m_html_builder );

    ASSERT( m_request_parser != NULL );
    FREE_POINTER( m_request_parser );

    free_buffer( );
}

//--------------------------------------------------------------------------------------------------

void webclient_handler_t::free_buffer( )
{
    ASSERT( m_buffer != NULL );
    free( m_buffer );
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void webclient_handler_t::do_run( )
{
    create_internal_objects( );

    while ( true )
    {
        scoped_ptr_t<socket_t> socket( m_clients_queue.pop( ) );
        if ( is_stopping( ) )
        {
            break;
        }

        m_socket = socket.get( );
        process_client( );

        m_socket = NULL;
        socket->shutdown( );
    }

    destroy_internal_objects( );
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void webclient_handler_t::do_stop( )
{
    socket_t *socket = new socket_t( );
    m_clients_queue.push( socket );
}

//--------------------------------------------------------------------------------------------------

void webclient_handler_t::process_client( )
{
    ASSERT( m_buffer != NULL ); ASSERT( m_request_parser != NULL ); ASSERT( m_socket != NULL );

    m_endpoint = m_socket->get_remote_endpoint( );

    size_t bytes_received = (size_t)m_socket->recv( m_buffer, m_buffer_size - 1 );
    if ( bytes_received == 0 )
    {
        // LOG_ERROR("Could not receive data from web client %s. Dropping ..", m_endpoint.c_str());
        return;
    }

    std::string request;
    WEBCLIENT_REQUEST_TYPE req_type = m_request_parser->parse( m_buffer, bytes_received, request );

    if ( std::string::npos == request.find( "?c=0:" ) )
    {
        LOG_TRACE( "Web client %s connected -> %s", m_endpoint.c_str( ), request.c_str( ) );
    }

    switch ( req_type )
    {
    case WRT_COMMAND:
        process_command_request( request );
        break;
    case WRT_FAVICON:
        process_favicon_request( );
        break;
    case WRT_JQUERY:
        process_jquery_request( );
        break;
    case WRT_INDEX:
        process_index_request( );
        break;
    default:
        process_invalid_request( );
    }
}

//--------------------------------------------------------------------------------------------------

bool webclient_handler_t::process_favicon_request( )
{
    std::string headers;
    m_html_builder->build_favicon_headers( sizeof( g_aszFaviconContent ), headers );

    return send_response( headers.c_str( ), headers.length( ) )&&
        send_response( g_aszFaviconContent, sizeof( g_aszFaviconContent ) );
}

//--------------------------------------------------------------------------------------------------

bool webclient_handler_t::process_jquery_request( )
{
    std::string headers;
    m_html_builder->build_jquery_headers( sizeof( g_aszJQueryContent ) - 1, headers );

    return send_response( headers.c_str( ), headers.length( ) )&&
        send_response( g_aszJQueryContent, sizeof( g_aszJQueryContent ) - 1 );
    // some invalid character in the end
}

//--------------------------------------------------------------------------------------------------

bool webclient_handler_t::process_index_request( )
{
    std::string request = "?c=0:l=0";
    std::string response;
    if ( !execute_request( request, response ) )
    {
        return false;
    }

    std::string full_content;
    StrUtils::FormatString( full_content, g_aszIndexContent, response.c_str( ) );

    std::string headers;
    m_html_builder->build_headers( full_content.length( ), headers );

    return send_response( headers.c_str( ), headers.length( ) )&&
        send_response( full_content.c_str( ), full_content.length( ) );
}

//--------------------------------------------------------------------------------------------------

bool webclient_handler_t::process_command_request( std::string& request )
{
    std::string response;
    if ( !execute_request( request, response ) )
    {
        return false;
    }

    std::string headers;
    m_html_builder->build_headers( response.length( ), headers );

    return send_response( headers.c_str( ), headers.length( ) )&&
        send_response( response.c_str( ), response.length( ) );
}

//--------------------------------------------------------------------------------------------------

void webclient_handler_t::process_invalid_request( )
{
    LOG_ERROR( "Incorrect HTTP request: %s", m_buffer );
}

//--------------------------------------------------------------------------------------------------

bool webclient_handler_t::send_response( const void *data, size_t len ) const
{
    ASSERT( m_socket != NULL ); ASSERT( !m_endpoint.empty( ) );

    if ( !m_socket->send( (void *)data, len ) )
    {
        LOG_ERROR( "Could not send response to web client %s. Dropping ..", m_endpoint.c_str( ) );
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

bool webclient_handler_t::execute_request( const std::string& request, std::string& response )
{
    device_command_t comamnd;
    if ( !m_request_parser->to_device_command( request, comamnd ) )
    {
        return false;
    }

    device_state_t state;
    m_request_executor.execute( comamnd, state );
    m_html_builder->convert_to_html( state, response );

    if ( std::string::npos != request.find( ":q" ) )
    {
        response += "<script>self.close();</script>";
    }

    cout << response.c_str( ) << endl;

    return true;
}

//--------------------------------------------------------------------------------------------------
