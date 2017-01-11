#pragma once

#include <command/command_factory.h>
#include <command/command_handler.h>
#include <common/driver_intf.h>

#include <lock_queue.h>
#include <threading/threading.h>

#include <memory>


class config_t;
class light_object_t;

namespace smarty
{
    class command_t;
}

namespace Json
{
    class Value;
}


class command_processor_t : public thread_base_t, public smarty::command_handler_t,
                          public smarty::command_factory_t
{
public:
    command_processor_t( driver_intf_t& driver, const config_t& config,
                       std::vector< light_object_t >& lights );
    ~command_processor_t( );

public: // thread_base_t
    virtual void do_run( );
    virtual void do_stop( );

public: // smarty::command_handler_t
    void add_command( std::shared_ptr< smarty::command_t > cmd );

public: // smarty::command_factory_t
    virtual std::shared_ptr< smarty::command_t >
    create_device_command( const device_command_t& cmd, uint timeout );

private:
    void process_command( );
    void check_light_objects( );

private:
    driver_intf_t& m_driver;
    const config_t& m_config;
    std::vector< light_object_t >& m_lights;

    lock_queue_t< std::shared_ptr< smarty::command_t > > m_cmd_queue;
};
