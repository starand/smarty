#ifndef __DEVICE_DRIVER_H_INCLUDED
#define __DEVICE_DRIVER_H_INCLUDED

#include <common/driver_intf.h>


class spi_t;
class gpio_t;
class mutex_t;

class device_driver_t
{
public:
    device_driver_t( );
    ~device_driver_t( );

private:
    void create_internal_objects( );
    void destroy_internal_objects( );

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

#endif // __DEVICE_DRIVER_H_INCLUDED
