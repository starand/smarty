#include <common/StdAfx.h>
#include "html_builder.h"

#include <common/smarty_config.h>
#include <common/driver_intf.h>
#include <common/enums.h>

#include <strutils.h>


static const char g_szLightNode[] = "light";
static const char g_szStatusNode[] = "status";

// headers
static const char g_szPacketFormat[] =
    "HTTP/1.0 200 OK\nContent-Type: text/html\nContent-Length: %u\n\n%s";
static const char g_szHeadersFormat[] =
    "HTTP/1.0 200 OK\nContent-Type: text/html\nContent-Length: %u\n\n";
static const char g_szFaviconHeadersFormat[] =
    "HTTP/1.1 200 OK\nAccept-Ranges: bytes\nContent-Length: %u\nContent-Type: image/x-icon\n\n";
static const char g_szJQueryHeadersFormat[] =
    "HTTP/1.1 200 OK\nAccept-Ranges: bytes\nContent-Length: %u\n"
    "Content-Type: application/javascript\n\n";

// table html
static const char g_szTableBegin[] =
    "<table style='width:100%;height:100%;' cellpadding=0 cellspacing=5>\n";
static const char g_szTableEnd[] = "</table>\n";
static const char g_szRowBegin[] = "<tr><td style='width:50%; vertical-align: middle;'>";
static const char g_szRowMiddle[] = "</td><td>";
static const char g_szRowEnd[] = "</td></tr>\n";

// button html
static const char g_szButtonFormat[] =
    "<div id='%s' style='height: 100%; width:100%; background-color: %s; font-color:white; "
    "font-size: 24px; text-align: center; vertical-align: middle; -moz-border-radius: 15px; "
    " -webkit-border-radius: 15px;'><div style='position:relative; top: 50%; color: white;'>"
    "%s</div></div>\n";

static const char g_szHandlerFormat[] =
    "$('#%s').click(function() { $('#content').load('/?c=%u:l=%u'); })\n;";
static const char g_szHandlersBegin[] = "<script>\n$(document).ready(function() {\n";
static const char g_szHandlersEnd[] =
    "\n})\nsetTimeout( 'refresh();', 1000 );\n"
    "function refresh() { $('#content').load('/?c=0:l=0'); }\n</script>";


//--------------------------------------------------------------------------------------------------

html_builder_t::html_builder_t( smarty_config_t& config )
    : m_config( config )
{
}

//--------------------------------------------------------------------------------------------------

html_builder_t::~html_builder_t( )
{
}

//--------------------------------------------------------------------------------------------------

void html_builder_t::build_headers( size_t len, std::string& headers )
{
    StrUtils::FormatString( headers, g_szHeadersFormat, len );
}

//--------------------------------------------------------------------------------------------------

bool html_builder_t::convert_to_html( const device_state_t& state, std::string& html_content )
{
    lights_state_t light_status = state.lights;

    html_content = g_szTableBegin;

    int buttons_count = 0;
    std::string handlers_content;

    for ( size_t pin = 0; pin < 8; ++pin )
    {
        std::string button_name = m_config.get_light_name( pin + 1 );
        if ( button_name.empty( ) ) continue;

        html_content.append( ( buttons_count % 2 == 0 ) ? g_szRowBegin : g_szRowMiddle );

        bool pin_is_on = ( ( 1 << pin ) & light_status ) != 0;

        std::string button;
        StrUtils::FormatString( button, g_szButtonFormat, button_name.c_str( ),
            ( pin_is_on ? "#b46c16" : "#252525" ), button_name.c_str( ) );
        html_content.append( button );

        if ( buttons_count % 2 == 1 )
        {
            html_content.append( g_szRowEnd );
        }

        std::string handler;
        StrUtils::FormatString( handler, g_szHandlerFormat, button_name.c_str( ),
            ( pin_is_on ? EC_TURNOFF : EC_TURNON ), ( 1 << pin ) );
        handlers_content.append( handler );

        ++buttons_count;
    }

    html_content.append( g_szTableEnd );

    html_content.append( g_szHandlersBegin );
    html_content.append( handlers_content );
    html_content.append( g_szHandlersEnd );

    return true;
}

//--------------------------------------------------------------------------------------------------

void html_builder_t::build_favicon_headers( size_t len, std::string& headers )
{
    StrUtils::FormatString( headers, g_szFaviconHeadersFormat, len );
}

//--------------------------------------------------------------------------------------------------

void html_builder_t::build_jquery_headers( size_t len, std::string& headers )
{
    StrUtils::FormatString( headers, g_szJQueryHeadersFormat, len );
}

//--------------------------------------------------------------------------------------------------
