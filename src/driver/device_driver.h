#pragma once

#include <driver/driver.h>
#include <map>

#define PACKET_SIZE     8

class i2c_master_t;

class device_driver_t : public driver_t
{
public:
    device_driver_t( );
    ~device_driver_t( );

public:
    bool init( const char* dev /*= "/dev/i2c-1"*/ );

    bool execute_command( const device_command_t& command, device_state_t& state ) override;

private:
    void update_state( device_state_t& state ) const;

    void dump_packet( char packet[] ) const;

private:
    std::map< int, i2c_master_t > m_slaves;

    char m_send_buffer[ PACKET_SIZE ] = { 0 };
    char m_recv_buffer[ PACKET_SIZE ] = { 0 };
};
