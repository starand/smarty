#pragma once

#include <common/enums.h>

#include <memory>
#include <vector>


class config_t;

namespace smarty
{
    class command_factory_t;
    class command_t;
    class event_t;
    class event_factory_t;
}

namespace Json
{
    class Value;
}


typedef std::shared_ptr< smarty::command_t > action_t;
typedef std::vector< action_t > actions_t;


class event_parser_t
{
public:
    event_parser_t( smarty::event_factory_t& event_factory,
                    smarty::command_factory_t& command_factory );
    ~event_parser_t( );

    bool parse( const config_t& config );

    static const std::vector< std::string > get_modes( );

private:
    bool parse_event( Json::Value node );

    bool parse_mode( Json::Value node, uint& mode );
    uint get_mode_id( const std::string& mode_name );

    std::string parse_mode_text( const std::string& condition, uint& mode );
    std::string parse_timeout_text( const std::string& action, uint& timeout );

    bool parse_actions( Json::Value node, actions_t& actions );
    bool parse_actions_json( Json::Value node, actions_t& actions );
    bool parse_action_json( Json::Value action, actions_t& actions);
    bool parse_actions_text( Json::Value node, actions_t& actions );
    bool parse_action_text( Json::Value node, actions_t& actions );
    bool parse_actions_array_text( Json::Value node, actions_t& actions );

    bool parse_condition( Json::Value node, std::shared_ptr< smarty::event_t >& handler );
    bool parse_condition_json( Json::Value node, std::shared_ptr< smarty::event_t >& event );
    bool parse_condition_text( Json::Value node, std::shared_ptr< smarty::event_t >& event);

    bool parse_device_condition( Json::Value node, std::shared_ptr< smarty::event_t >& handler,
                                 uint mode );
    bool parse_mode_condition( Json::Value node, std::shared_ptr< smarty::event_t >& handler );

    uint parse_sensor_pin( Json::Value node );
    uint parse_sensor_pin( const std::string& name );
    uint parse_light_pin( Json::Value node );
    uint parse_light_pin( const std::string& name );

    bool parse_modes( Json::Value node );

    void add_device_command( actions_t& actions,
                             device_cmd_t cmd, device_param_t param, uint timeout = 0 );

private:
    smarty::event_factory_t& m_event_factory;
    smarty::command_factory_t& m_command_factory;

    static std::vector< std::string > m_modes;

    std::map< std::string, uint > m_sensors;
    std::map< std::string, uint > m_lights;
};
