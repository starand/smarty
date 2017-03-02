#pragma once

#include <common/driver_intf.h>
#include <driver/driver.h>

#include <threading.h>
#include <memory>
#include <set>


class device_controller_t : public driver_intf_t, public thread_base_t
{
public:
    device_controller_t( driver_t& device );
    ~device_controller_t( );

public: // driver_intf_t implementation
    virtual ErrorCode execute_command( const device_command_t& command );
    virtual ErrorCode get_state( device_state_t& state ) const;

    virtual ErrorCode add_observer( device_observer_t& observer );
    virtual ErrorCode remove_observer( device_observer_t& observer );

public: // thread_base_t implementation
    virtual void do_run( );
    virtual void do_stop( );

private:
    void do_execute_command( const device_command_t& command );
    void update_light_times( uint bitset );

private:
    driver_t& m_device;
    mutex_t m_device_lock;

    std::set< device_observer_t* > m_observers_list;

    device_state_t m_device_state;
    std::vector< time_t > m_light_times;
    event_t m_wait_timeout;
};
