#ifndef __REQUEST_PARSER_H_INCLUDED
#define __REQUEST_PARSER_H_INCLUDED


struct device_command_t;

enum WEBCLIENT_REQUEST_TYPE
{
    WRT__MIN = 0,

    WRT_COMMAND = WRT__MIN,
    WRT_FAVICON,
    WRT_JQUERY,
    WRT_INDEX,

    WRT__MAX,
    WRT_INVALID = WRT__MAX,
};


class request_parser_t
{
public:
    request_parser_t( );
    ~request_parser_t( );

public:
    WEBCLIENT_REQUEST_TYPE parse( char *content, size_t length, std::string& request ) const;
    bool to_device_command( const std::string& request, device_command_t& command ) const;

private:
    bool parse_method_header( const char *header, std::string& request ) const;

private:
    // typedef std::map<const char*, const char*> headers_map_t;
};

#endif // __REQUEST_PARSER_H_INCLUDED
