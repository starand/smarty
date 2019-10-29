#include <common/StdAfx.h>
#include "device_driver.h"

#include <common/enums.h>

#include <threading.h>
#include <utils.h>

#include <raspberry/i2c_master.h>


#define COMMAND_LENGTH	6
#define LIGHTS_BYTE		3
#define BUTTONS_BYTE	4
#define IR_BYTE			5
#define SENSORS_BYTE	2

#define RESET_DELAY			100
#define RETRY_COUNT			3

//--------------------------------------------------------------------------------------------------

#define REQ_CMD			0
#define REQ_PARAM		1

//#define NEED_TO_DUMP_REQUEST
//#define NEED_TO_DUMP_RESPONSE

#ifdef NEED_TO_DUMP_REQUEST
#	define DUMP_REQUEST() dump_packet(m_send_buffer);
#else
#	define DUMP_REQUEST()
#endif

#ifdef NEED_TO_DUMP_RESPONSE
#	define DUMP_RESPONSE() dump_packet(m_recv_buffer);
#else
#	define DUMP_RESPONSE()
#endif

//--------------------------------------------------------------------------------------------------

lights_state_t g_previous_state = 0;

//--------------------------------------------------------------------------------------------------

device_driver_t::device_driver_t(  )
    : m_slaves( )
{
    ASSERT( m_slaves.empty( ) );

    int slave_addr = 0x50;
    m_slaves.insert( { slave_addr, i2c_master_t( slave_addr ) } );
    slave_addr = 0x51;
    m_slaves.insert( { slave_addr, i2c_master_t( slave_addr ) } );
}

//--------------------------------------------------------------------------------------------------

device_driver_t::~device_driver_t( )
{
    for( auto& slave : m_slaves ) {
        slave.second.close( );
    }
}

//--------------------------------------------------------------------------------------------------

bool
device_driver_t::init( const char* dev /*= "/dev/i2c-1"*/ )
{
    for( auto& slave : m_slaves ) {
        if ( !slave.second.init( dev ) || !slave.second.connect( ) ) {
            return false;
        }
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

bool device_driver_t::execute_command( const device_command_t& command, device_state_t& state )
{
    ASSERT( !m_slaves.empty( ) );

/*
    int tries = 0;
    do
    {
        EXECUTE_REQUEST( command.cmd, command.param );

        if ( check_correctness( ) )
        {
            break;
        }

        if ( ++tries == RETRY_COUNT )
        {
            do_reset( );
            return false;
        }
    }
    while ( true );

    update_state( state );
*/
    return true;
}

//--------------------------------------------------------------------------------------------------

void device_driver_t::update_state( device_state_t& state ) const
{
    state.lights = m_recv_buffer[ LIGHTS_BYTE ];
    state.buttons = m_recv_buffer[ BUTTONS_BYTE ];
    state.sensors = m_recv_buffer[ SENSORS_BYTE ];

    g_previous_state = state.lights;
}

//--------------------------------------------------------------------------------------------------

void device_driver_t::dump_packet( char packet[] ) const
{
    for ( int i = 0; i < PACKET_SIZE; ++i )
    {
        cout << hex << setw( 2 ) << setfill( '0' ) << (int)packet[ i ] << ' ';
    }
}

//--------------------------------------------------------------------------------------------------
