#pragma once

#include <common/driver_intf.h>


class spi_t;
class gpio_t;
class mutex_t;

class driver_t
{
public:
    driver_t( );
    ~driver_t( );

public:
    bool execute_command( const device_command_t& command, device_state_t& state );

private:
    bool check_correctness( ) const;
    void do_reset( );

    void update_state( device_state_t& state ) const;

    void dump_packet( char packet[] ) const;

private:
    spi_t *m_spi;
    gpio_t *m_reset_gpio;

    size_t m_reset_count;
};
