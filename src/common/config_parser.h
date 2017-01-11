#ifndef __CONFIG_PARSER_H_INCLUDED
#define __CONFIG_PARSER_H_INCLUDED

#include <json/value.h>


struct light_info_t
{
    string name;
    sizeint pin;
    bool automatic_set;
};


class config_parser_t
{
public:
    config_parser_t( const string& config );
    ~config_parser_t( );

    bool parse( );
    bool is_parsed( ) const;

    sizeint get_lights_count( ) const;
    bool get_light_info( sizeint index, light_info_t& info ) const;

    const Json::Value& operator[]( const std::string& name ) const;

private:
    Json::Value m_lights;
    bool m_parsed;
    string m_config;
};

#endif // __CONFIG_PARSER_H_INCLUDED
