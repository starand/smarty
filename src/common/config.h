#pragma once

#include <files/config.h>
#include <map>


class config_t : public FileUtils::config_t
{
public:
    config_t( );
    ~config_t( ) = default;

    bool do_parse( ) override;

public:
    uint get_light_pin( const string& name ) const;
    string get_light_name( uint pin ) const;
    string get_light_names( uchar pins ) const;

    string get_sensor_name( uint pin ) const;
    string get_sensor_names( uchar pins ) const;

    const Json::Value& operator[]( const std::string& name ) const override;

private:
    bool parse_lights( );
    bool parse_sensors( );

private:
    typedef std::map< uint, std::string > pin_config_t;

private:
    pin_config_t m_light_pins_map;
    pin_config_t m_sensor_pins_map;
};
