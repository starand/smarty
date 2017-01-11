#pragma once

#include <common/enums.h>
#include <memory>


namespace smarty
{

class event_t;

//--------------------------------------------------------------------------------------------------

class event_factory_t
{
public:
    virtual ~event_factory_t( ) { }

    virtual std::shared_ptr< smarty::event_t >
    create_sensor_event( uint pin, TriggerState state, uint mode ) = 0;

    virtual std::shared_ptr< smarty::event_t >
    create_button_event( uint pin, TriggerState state, uint mode ) = 0;

    virtual std::shared_ptr< smarty::event_t >
    create_light_event( uint pin, TriggerState state, uint mode ) = 0;

    virtual std::shared_ptr< smarty::event_t >
    create_mode_event( uint mode, bool enabled ) = 0;
};

//--------------------------------------------------------------------------------------------------

}
