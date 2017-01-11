#include <common/StdAfx.h>

#include <command/command_processor.h>
#include <command/command_device.h>
#include <device/light_object.h>

#include <common/errors.h>
#include <common/mc_enum.h>
#include <files/config.h>

#include <memory>


namespace
{

//--------------------------------------------------------------------------------------------------

uint get_bit_offset( device_param_t param )
{
    uint result = 0;
    while ( param && ( param & 1 ) == 0 )
    {
        ++result;
        param >>= 1;
    }

    return result;
}

//--------------------------------------------------------------------------------------------------

}


command_processor_t::command_processor_t( driver_intf_t& driver, const config_t& config,
                                          std::vector< light_object_t >& lights )
    : m_driver( driver )
    , m_config( config )
    , m_lights( lights )
    , m_cmd_queue( )
{
}

//--------------------------------------------------------------------------------------------------

command_processor_t::~command_processor_t( )
{
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void command_processor_t::do_run( )
{
    while ( true )
    {
        sleep( 100 );
        if ( is_stopping( ) )
        {
            break;
        }

        if ( !m_cmd_queue.empty( ) )
        {
            process_command( );
        }

        check_light_objects( );
    }
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void command_processor_t::do_stop( )
{
    class fake_command : public smarty::command_t
    {
    public:
        virtual ErrorCode execute( driver_intf_t& driver ) { return ErrorCode::OK; }
    };

    std::shared_ptr< smarty::command_t > fake( new fake_command( ) );
    m_cmd_queue.push( fake );
}

//--------------------------------------------------------------------------------------------------

void command_processor_t::add_command( std::shared_ptr< smarty::command_t > cmd )
{
    m_cmd_queue.push( cmd );
    LOG_TRACE( "[cmd.%p] Command added to command handler", cmd.get( ) );
}

//--------------------------------------------------------------------------------------------------

std::shared_ptr< smarty::command_t >
command_processor_t::create_device_command( const device_command_t& cmd, uint timeout )
{
    uint idx = get_bit_offset( cmd.param );
    auto res = std::make_shared< command_device_t >( cmd, timeout, m_lights[ idx ] );
    LOG_DEBUG( "[cmd.%p] Device command [%u:%u] with params (timeout %u) created",
               res.get(), cmd.cmd, cmd.param, timeout );

    return res;
}

//--------------------------------------------------------------------------------------------------

void command_processor_t::process_command( )
{
    auto command = m_cmd_queue.pop( );
    ErrorCode code = command->execute( m_driver );

    if ( code != ErrorCode::OK )
    {
        LOG_ERROR( "[cmd.%p] Command failed with code %u", command.get( ), code );
    }

    LOG_TRACE( "[cmd.%p] Command processed", command.get( ) );
}

//--------------------------------------------------------------------------------------------------

void command_processor_t::check_light_objects( )
{
    time_t current = time( nullptr );

    uint count = m_lights.size( );
    for ( uint idx = 0; idx < count; ++idx )
    {
        auto& light = m_lights[ idx ];

        time_t timeout = light.get_turnoff_timeout( );
        if ( timeout && timeout <= current )
        {
            device_command_t cmd{ EC_TURNOFF, static_cast<device_param_t>( 1 << idx ) };
            add_command( std::make_shared< command_device_t >( cmd, 0, light ) );

            light.clear_turnoff_time( );
        }
    }
}

//--------------------------------------------------------------------------------------------------
