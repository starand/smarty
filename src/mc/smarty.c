#include <avr/io.h>
#include <avr/iom32.h>
#include <avr/interrupt.h>
#include "command_enum.h"


#define BUFFER_SIZE	6
#define END_DATA	0xFF
#define true		1
#define false		0

typedef unsigned char uchar;

uchar recv_buffer[BUFFER_SIZE];
uchar send_buffer[BUFFER_SIZE] = "\x00\x00\x00\x00\x00\xFF";

volatile int pos = 0;
volatile char process_it = false;
volatile uchar btn_state = 0;
volatile uchar alter_state = 0;

void SPI_SlaveInit(void)
{
	DDRB |= 1 << PB6; // Set MISO to output
	SPCR = (1 << SPE) | (1 << SPIE); // Enable SPI & interrupts
	sei();
}

uchar SPI_SlaveSendReceive(uchar set_data)
{
	SPDR = set_data;
	while (!(SPSR & (1 << SPIF)));
	return SPDR;
}

ISR(SPI_STC_vect)
{
	SPDR = send_buffer[pos];
	uchar data = SPDR;

	if (pos < BUFFER_SIZE)
	{
		recv_buffer[pos++] = data;
		if (data == END_DATA)
		{
			process_it = true;
		}
	}
	else
	{
		pos = 0;
	}
}

void update_lights();
void process_command();
int check_command();
void execute_command();

int main(void)
{
	SPI_SlaveInit();
	// disable JTAG to be able use PORTC 2-5 pins as IO
	MCUCSR = (1<<JTD);
	MCUCSR = (1<<JTD);
	
	// PORTC - output, PORTD - input, high
	DDRC = 0xFF;
	DDRD = 0x00;
	PORTD = 0xFF;
	
	while (1)
	{
		update_lights();
		process_command();
	}
}


#define CHECK_BTN_BIT(_PIN_) \
    if (bit_is_clear(PIND, 7 - _PIN_)) btn_state |= 1 << _PIN_;

#define CHECK_BUTTONS() \
    btn_state = 0; \
    CHECK_BTN_BIT(0); CHECK_BTN_BIT(1); CHECK_BTN_BIT(2); CHECK_BTN_BIT(3); \
    CHECK_BTN_BIT(4); CHECK_BTN_BIT(5); CHECK_BTN_BIT(6); CHECK_BTN_BIT(7);

#define IS_ON(_PIN_) ((send_buffer[2] & (1 << _PIN_)) ? 1 : 0)
#define IS_OFF(_PIN_) ((send_buffer[2] & (1 << _PIN_)) ? 0 : 1)

#define TOGGLE_PIN(_PIN_) alter_state ^= (1 << _PIN_)
#define TURN_ON(_PIN_) { if (IS_OFF(_PIN_)) TOGGLE_PIN(_PIN_); }
#define TURN_OFF(_PIN_) { if (IS_ON(_PIN_)) TOGGLE_PIN(_PIN_); }

#define BIT_SET(_BYTE_, _BIT_) ((_BYTE_ & (1 << _BIT_)) ? 1 : 0)
#define TEST_AND_TURN_ON(_BYTE_, _BIT_) { if (BIT_SET(_BYTE_, _BIT_)) TURN_ON(_BIT_); }
#define TEST_AND_TURN_OFF(_BYTE_, _BIT_) { if (BIT_SET(_BYTE_, _BIT_)) TURN_OFF(_BIT_); }

void update_lights()
{
	CHECK_BUTTONS();
    uchar light_state = btn_state ^ alter_state;
    PORTC = light_state;
    send_buffer[2] = light_state;
    send_buffer[3] = btn_state;
}

void process_command()
{
	if (process_it)
	{
        if (check_command())
        {
            execute_command();
        }
        else
        {
            // if command is incorrect sed such resposne
            send_buffer[0] = send_buffer[1] = 0xEE;
        }

		pos = 0;
		process_it = false;
	}
}

int check_command()
{
    return (pos == 6 && recv_buffer[2] == 0 && recv_buffer[3] == 0
            && recv_buffer[4] == 0 && recv_buffer[5] == 0xFF) ? 1 : 0;
}


void execute_command()
{
    int idx = 0;
    uchar param = recv_buffer[1];

    switch (recv_buffer[0])
    {
    case EC_STATUS:
        break;
    case EC_TURNON:
        for (idx = 0; idx < 8; ++idx)
        {
            TEST_AND_TURN_ON(param, idx);
        }
        break;
    case EC_TURNOFF:
        for (idx = 0; idx < 8; ++idx)
        {
            TEST_AND_TURN_OFF(param, idx);
        }
        break;
    case EC_TURNALL:
        for (idx = 0; idx < 8; ++idx)
        {
            if (param == 0x00)
			{
				TURN_OFF(idx);
			}
			else
			{
				TURN_ON(idx);
			}
        }
        break;
    }
}

