#pragma once

#include <common/mc_enum.h>
#include <driver/driver.h>


class driver_mock_t : public driver_t
{
public:
    driver_mock_t( ) : m_state( { 0, 0, 0 } )
    {
    }

public:
    bool execute_command( const device_command_t& command, device_state_t& state ) override
    {
        switch ( command.cmd )
        {
        case EC_STATUS:
            break;
        case EC_TURNON:
            m_state.lights ^= command.param;
            break;
        case EC_TURNOFF:
            m_state.lights &= ~command.param;
            break;
        case EC_TURNALL:
            m_state.lights = command.param == 1 ? 0xFF : 0x00;
            break;
        case EC_TURNEQUAL:
            break;
        case EC_TURNOFFBUTTON:
            break;
        case EC_TURNONBUTTON:
            break;
        }

        state = m_state;
        return true;
    }

public:
    device_state_t m_state;
};
