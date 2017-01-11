#include <avr/io.h>
#include <avr/iom32.h>
#include <avr/interrupt.h>

#include <common/mc_enum.h>


#define BUFFER_SIZE	6
#define END_DATA	0xFF
#define true		1
#define false		0


#define LIGHTS_DDR	DDRA
#define LIGHTS_PORT	PORTA
#define BUTTONS_DDR	DDRC
#define BUTTONS_PORT	PORTC
#define BUTTON_PIN	PINC

#define IRPOS	4


#define BUTTONS_STATE() \
   ( (bit_is_clear(BUTTON_PIN, 0) ? 128 : 0) | (bit_is_clear(BUTTON_PIN, 1) ? 64 : 0) | \
   (bit_is_clear(BUTTON_PIN, 2) ? 32 : 0) | (bit_is_clear(BUTTON_PIN, 3) ? 16 : 0) | \
   (bit_is_clear(BUTTON_PIN, 4) ? 8 : 0) | (bit_is_clear(BUTTON_PIN, 5) ? 4 : 0) | \
   (bit_is_clear(BUTTON_PIN, 6) ? 2 : 0) | (bit_is_clear(BUTTON_PIN, 7) ? 1 : 0) )

#define SENSORS_STATE( ) \
    (bit_is_clear(PINB, 0) ? 1 : 0) | (bit_is_clear(PINB, 1) ? 2 : 0) | \
    (bit_is_clear(PINB, 2) ? 4 : 0) | (bit_is_clear(PINB, 3) ? 8 : 0) | \
    (bit_is_clear(PIND, 4) ? 16 : 0) | (bit_is_clear(PIND, 5) ? 32 : 0) | \
    (bit_is_clear(PIND, 6) ? 64 : 0) | (bit_is_clear(PIND, 7) ? 128 : 0)


typedef unsigned char uchar;
typedef unsigned long uint32;

uchar recv_buffer[ BUFFER_SIZE ];
uchar send_buffer[ BUFFER_SIZE ] = "\x00\x00\x00\x00\x00\xFF";

volatile int pos = 0;
volatile char process_it = false;

volatile uint32 buttons_state = 0;
volatile uint32 buttons_disabled_state = 0;
volatile uint32 lights_state = 0;
volatile uint32 sensors_state = 0;


void SPI_SlaveInit( void )
{
    DDRB |= 1 << PB6; // Set MISO to output
    SPCR = ( 1 << SPE ) | ( 1 << SPIE ); // Enable SPI & interrupts
    sei( );
}

uchar SPI_SlaveSendReceive( uchar set_data )
{
    SPDR = set_data;
    while ( !( SPSR & ( 1 << SPIF ) ) );
    return SPDR;
}

ISR( SPI_STC_vect )
{
    SPDR = send_buffer[ pos ];
    uchar data = SPDR;

    if ( pos < BUFFER_SIZE )
    {
        recv_buffer[ pos++ ] = data;
        if ( data == END_DATA )
        {
            process_it = true;
        }
    }
    else
    {
        pos = 0;
    }
}

void read_sensors( );
void update_lights( );
void set_buffer( );
void process_command( );
int check_command( );
void execute_command( );

int main( void )
{
    DDRB = 0x00;
    PORTB = 0xFF;
    DDRD = 0x00;
    PORTD = 0xFF;

    SPI_SlaveInit( );
    // disable JTAG to be able use PORTC 2-5 pins as IO
    MCUCSR = ( 1 << JTD );
    MCUCSR = ( 1 << JTD );

    // LIGHTS_PORT - output, BUTTONS_PORT - input, high
    LIGHTS_DDR = 0xFF;
    LIGHTS_PORT = 0x00;
    BUTTONS_DDR = 0x00;
    BUTTONS_PORT = 0xFF;

    lights_state = 0x00;
    buttons_state = ~BUTTONS_PORT;

    while ( 1 )
    {
        update_lights( );
        read_sensors( );
        set_buffer( );
        process_command( );
    }
}

void read_sensors( )
{
    uint32 curr_sensors_state = SENSORS_STATE( );
    if ( curr_sensors_state != sensors_state )
    {
        sensors_state = curr_sensors_state;
    }
}

void update_lights( )
{
    // read buttons state
    auto current_buttons_state = BUTTONS_STATE( );
    auto buttons_diff = ( buttons_state ^ current_buttons_state ) & ~buttons_disabled_state;
    buttons_state = current_buttons_state;

    if ( buttons_diff )
    {
        lights_state ^= buttons_diff;
    }

    LIGHTS_PORT = lights_state;
}

void set_buffer( )
{
    send_buffer[ 1 ] = sensors_state & 0xFF;
    send_buffer[ 2 ] = lights_state & 0xFF;
    send_buffer[ 3 ] = buttons_state & 0xFF;
}

void process_command( )
{
    if ( process_it )
    {
        if ( check_command( ) )
        {
            execute_command( );
        }
        else
        {
            // if command is incorrect set such response
            send_buffer[ 0 ] = send_buffer[ 1 ] = 0xEE;
        }

        pos = 0;
        send_buffer[ IRPOS ] = 0;
        process_it = false;
    }
}

int check_command( )
{
    return ( pos == 6 && recv_buffer[ 2 ] == 0 && recv_buffer[ 3 ] == 0
        && recv_buffer[ 4 ] == 0 && recv_buffer[ 5 ] == 0xFF ) ? 1 : 0;
}


void execute_command( )
{
    int idx = 0;

    auto cmd = recv_buffer[ 0 ];
    auto param = recv_buffer[ 1 ];

    switch ( cmd )
    {
    case EC_STATUS:
        break;
    case EC_TURNON:
        lights_state |= param;
        break;
    case EC_TURNOFF:
        lights_state &= ~param;
        break;
    case EC_TURNALL:
        lights_state = param == 0 ? 0 : ~( (uint32)0 );
        break;
    case EC_TURNEQUAL:
        break;
    case EC_TURNOFFBUTTON:
        buttons_disabled_state |= param;
        break;
    case EC_TURNONBUTTON:
        buttons_disabled_state &= ~param;
        break;
    }
}

