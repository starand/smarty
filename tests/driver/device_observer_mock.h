#pragma once

#include <common/driver_intf.h>
#include <iostream>


class device_observer_mock_t : public device_observer_t
{
public:
    device_observer_mock_t( ) : light_changed_count( 0 )
    {
    }

public:
    virtual void on_light_changed( const lights_state_t& bitset )
    {
        ++light_changed_count;
    }

    virtual void on_button_pressed( const buttons_state_t& bitset )
    {
        std::cout << "on_button_pressed" << std::endl;
    }

    virtual void on_sensor_triggered( const sensors_state_t& bitset )
    {
        std::cout << "on_sensor_triggered" << std::endl;
    }

    virtual void on_double_click( uint button_pin )
    {
        std::cout << "on_double_click" << std::endl;
    }

public:
    uint light_changed_count;
};