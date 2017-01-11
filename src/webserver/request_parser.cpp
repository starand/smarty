#include <common/StdAfx.h>
#include "request_parser.h"

#include <common/driver_intf.h>

#include <strutils.h>
#include <string.h>
#include <stdlib.h>


static const char g_szGetMethod[] = "GET /";
static const size_t g_nGetMethodLen = strlen( g_szGetMethod );

static const char g_szFaviconRequest[] = "favicon.ico";
static const size_t g_szFaviconLength = strlen( g_szFaviconRequest );
static const char g_szJQueryRequest[] = "jquery.js";
static const size_t g_szJQueryLength = strlen( g_szJQueryRequest );
static const char g_szCommandRequest[] = "?";
static const size_t g_szCommandLength = strlen( g_szCommandRequest );


//--------------------------------------------------------------------------------------------------

request_parser_t::request_parser_t( )
{
}

//--------------------------------------------------------------------------------------------------

request_parser_t::~request_parser_t( )
{
}

//--------------------------------------------------------------------------------------------------

WEBCLIENT_REQUEST_TYPE request_parser_t::parse( char *content, size_t length,
                                                std::string& request ) const
{
    ASSERT( content != NULL ); ASSERT( length != 0 );
    content[ length ] = 0;

    char *current_pos = strchr( content, '\n' );
    if ( current_pos == NULL )
    {
        return WRT_INVALID;
    }

    *current_pos++ = 0;
    if ( !parse_method_header( content, request ) )
    {
        return WRT_INVALID;
    }

    if ( request.length( ) == 0 )
    {
        return WRT_INDEX;
    }
    else if ( memcmp( request.c_str( ), g_szFaviconRequest, g_szFaviconLength ) == 0 )
    {
        return WRT_FAVICON;
    }
    else if ( memcmp( request.c_str( ), g_szJQueryRequest, g_szJQueryLength ) == 0 )
    {
        return WRT_JQUERY;
    }
    else if ( memcmp( request.c_str( ), g_szCommandRequest, g_szCommandLength ) == 0 )
    {
        return WRT_COMMAND;
    }

    return WRT_INVALID;
}

//--------------------------------------------------------------------------------------------------

bool request_parser_t::to_device_command( const std::string& request,
                                          device_command_t& command ) const
{
    map_ss parsed_map;
    StrUtils::split_map( request.c_str( ) + 1, parsed_map, ":" ); // "+1" -- to skip ? character

    map_ss::iterator itCmd = parsed_map.find( "c" );
    if ( itCmd == parsed_map.end( ) )
    {
        LOG_ERROR( "Could not find cmd tag" );
        return false;
    }

    map_ss::iterator itLight = parsed_map.find( "l" );
    if ( itLight == parsed_map.end( ) )
    {
        LOG_ERROR( "Could not find light tag" );
        return false;
    }

    command.cmd = (device_cmd_t)atoi( itCmd->second.c_str( ) );
    command.param = (device_param_t)atoi( itLight->second.c_str( ) );

    return true;
}

//--------------------------------------------------------------------------------------------------

bool request_parser_t::parse_method_header( const char *header, std::string& request ) const
{
    ASSERT( header != NULL );

    if ( strncmp( header, g_szGetMethod, g_nGetMethodLen ) != 0 )
    {
        return false;
    }

    char *end_pos = strchr( (char*)header + g_nGetMethodLen, ' ' );
    if ( end_pos == NULL )
    {
        return false;
    }

    *end_pos++ = 0;

    request = header + g_nGetMethodLen;
    return true;
}

//--------------------------------------------------------------------------------------------------
