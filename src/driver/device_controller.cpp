#include <common/StdAfx.h>

#include <driver/device_controller.h>

#include <common/enums.h>

#include <utils.h>


#define POLL_TIMEOUT	50
#define MAX_LIGHT_COUNT 8


//--------------------------------------------------------------------------------------------------

device_controller_t::device_controller_t( driver_t& device )
    : m_device( device )
    , m_device_lock( )
    , m_observers_list( )
    , m_device_state( )
    , m_light_times( MAX_LIGHT_COUNT )
    , m_wait_timeout( false, false )
{
}

//--------------------------------------------------------------------------------------------------

device_controller_t::~device_controller_t( )
{
}

//--------------------------------------------------------------------------------------------------

/*virtual */
ErrorCode device_controller_t::execute_command( const device_command_t& command )
{
    do_execute_command( command );

    device_command_t status_command = { EC_STATUS, 0x00 };
    do_execute_command( status_command );

    return ErrorCode::OK;
}

//--------------------------------------------------------------------------------------------------

/*virtual */
ErrorCode device_controller_t::get_state( device_state_t& state ) const
{
    state = m_device_state;

    return ErrorCode::OK;
}

//--------------------------------------------------------------------------------------------------

/*virtual */
ErrorCode device_controller_t::add_observer( device_observer_t& observer )
{
    m_observers_list.insert( &observer );

    return ErrorCode::OK;
}

//--------------------------------------------------------------------------------------------------

/*virtual */
ErrorCode device_controller_t::remove_observer( device_observer_t& observer )
{
    m_observers_list.erase( &observer );

    return ErrorCode::OK;
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void device_controller_t::do_run( )
{
    while ( true )
    {
        if ( is_stopping( ) )
        {
            break;
        }

        device_command_t command = { EC_STATUS, 0x00 };
        do_execute_command( command );

        m_wait_timeout.wait( POLL_TIMEOUT );
    }
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void device_controller_t::do_stop( )
{
    m_wait_timeout.set( );
}

//--------------------------------------------------------------------------------------------------

void device_controller_t::do_execute_command( const device_command_t& command )
{
    device_state_t state;
    device_command_t cmd = { command.cmd, command.param };

    mutex_locker_t lock( m_device_lock );
    // two stage command execution needed because of 0xFF value MC returns incorrect state
    if ( cmd.param == 0xFF )
    {
        cmd.param = 0x0F;
        m_device.execute_command( cmd, state );
        cmd.param = 0xF0;
    }

    if ( m_device.execute_command( cmd, state ) && m_device_state != state )
    {
        if ( m_device_state.lights != state.lights )
        {
            update_light_times( m_device_state.lights ^ state.lights );

            for ( auto& observer : m_observers_list )
            {
                observer->on_light_changed( state.lights );
            }
        }

        if ( m_device_state.buttons != state.buttons )
        {
            for ( auto& observer : m_observers_list )
            {
                observer->on_button_pressed( state.buttons );
            }
        }

        if ( m_device_state.sensors != state.sensors )
        {
            for ( auto& observer : m_observers_list )
            {
                observer->on_sensor_triggered( state.sensors );
            }
        }

        m_device_state = state;
    }
}

//--------------------------------------------------------------------------------------------------

void device_controller_t::update_light_times( uint bitset )
{
    for ( uint idx = 0; idx < MAX_LIGHT_COUNT; ++idx )
    {
        uint bit = 1 << idx;
        if ( bitset & bit )
        {
            time_t current_time = time( nullptr );
            uint diff = difftime( current_time, m_light_times[ idx ] );
            if ( diff < 2 ) // 1 second
            {
                for ( auto& observer : m_observers_list )
                {
                    observer->on_double_click( idx );
                }
            }

            m_light_times[ idx ] = current_time;
        }
    }
}

//--------------------------------------------------------------------------------------------------
