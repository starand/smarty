#include <common/StdAfx.h>
#include "device_driver.h"

#include <common/enums.h>

#include <threading.h>
#include <utils.h>

#include <raspberry/spi.h>
#include <raspberry/gpio.h>


#define COMMAND_LENGTH	6
#define LIGHTS_BYTE		3
#define BUTTONS_BYTE	4
#define IR_BYTE			5
#define SENSORS_BYTE	2


#define RESET_GPIO			7
#define RESET_DELAY			100
#define RETRY_COUNT			3

//--------------------------------------------------------------------------------------------------

static char g_szRequestBuffer[ COMMAND_LENGTH ] = { 0x00, 0x00, 0x00, 0x00, 0x00, (char)0xFF };
static char g_szResponseBuffer[ COMMAND_LENGTH ] = { 0 };

#define REQ_CMD			0
#define REQ_PARAM		1
#define EXECUTE_REQUEST(_CMD_, _PARAM_) \
	g_szRequestBuffer[REQ_CMD] = _CMD_; \
	g_szRequestBuffer[REQ_PARAM] = _PARAM_; \
	DUMP_REQUEST(); \
	m_spi->read_write(g_szRequestBuffer, (char *)g_szResponseBuffer, COMMAND_LENGTH); \
	DUMP_RESPONSE();

//#define NEED_TO_DUMP_REQUEST
//#define NEED_TO_DUMP_RESPONSE

#ifdef NEED_TO_DUMP_REQUEST
#	define DUMP_REQUEST() if (g_szRequestBuffer[REQ_CMD] != 0) dump_packet(g_szRequestBuffer);
#else
#	define DUMP_REQUEST()
#endif

#ifdef NEED_TO_DUMP_RESPONSE
#	define DUMP_RESPONSE() dump_packet(g_szResponseBuffer);
#else
#	define DUMP_RESPONSE()
#endif

//--------------------------------------------------------------------------------------------------

lights_state_t g_previous_state = 0;

//--------------------------------------------------------------------------------------------------

device_driver_t::device_driver_t( )
    : m_spi( NULL )
    , m_reset_gpio( NULL )
    , m_reset_count( 0 )
{
    ASSERT( m_spi == NULL ); ASSERT( m_reset_gpio == NULL );

    m_spi = new spi_t( );
    m_reset_gpio = new  gpio_t( RESET_GPIO, OUTPUT, PUD_UP );
}

//--------------------------------------------------------------------------------------------------

device_driver_t::~device_driver_t( )
{
    ASSERT( m_spi != NULL ); ASSERT( m_reset_gpio != NULL );

    FREE_POINTER( m_spi );
    FREE_POINTER( m_reset_gpio );
}

//--------------------------------------------------------------------------------------------------

bool device_driver_t::execute_command( const device_command_t& command, device_state_t& state )
{
    ASSERT( m_spi != NULL );

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
    return true;
}

//--------------------------------------------------------------------------------------------------

bool device_driver_t::check_correctness( ) const
{
    return g_szResponseBuffer[ 0 ] == (char)0xFF && g_szResponseBuffer[ 5 ] == 0x00;
}

//--------------------------------------------------------------------------------------------------

void device_driver_t::do_reset( )
{
    LOG_ERROR( "Incorrect response. Reset #%u", ++m_reset_count );
    dump_packet( g_szResponseBuffer );

    m_reset_gpio->set( false ); // set LOW level to reset MC
    utils::sleep_ms( RESET_DELAY );

    m_reset_gpio->set( true ); // set HIGH - normal level
    utils::sleep_ms( RESET_DELAY );

    EXECUTE_REQUEST( EC_TURNOFF, ~g_previous_state );	// off previously disabled
    EXECUTE_REQUEST( EC_TURNON, g_previous_state );	// and on active
}

//--------------------------------------------------------------------------------------------------

void device_driver_t::update_state( device_state_t& state ) const
{
    state.lights = g_szResponseBuffer[ LIGHTS_BYTE ];
    state.buttons = g_szResponseBuffer[ BUTTONS_BYTE ];
    state.sensors = g_szResponseBuffer[ SENSORS_BYTE ];

    g_previous_state = state.lights;
}

//--------------------------------------------------------------------------------------------------

void device_driver_t::dump_packet( char packet[] ) const
{
    static size_t packet_sent = 0;

    for ( int i = 0; i < 6; ++i )
    {
        cout << hex << setw( 2 ) << setfill( '0' ) << (int)packet[ i ] << ' ';
    }

    cout << '[' << m_reset_count << "] " << ++packet_sent << endl;
}

//--------------------------------------------------------------------------------------------------
