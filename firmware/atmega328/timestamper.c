#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/twi.h>
#include <string.h>

#include "printf.h"
#include "i2c_master.h"
#include "spi.h"

/* Pin definitions */
#define SCLK_PORT	PORTB
#define SCLK_DDR	DDRB
#define SCLK_MASK	_BV(5)

#define SS_PORT	PORTB
#define SS_DDR	DDRB
#define SS_MASK	_BV(2)

#define MR_PORT	PORTC
#define MR_DDR	DDRC
#define MR_MASK	_BV(3)

#define CS_PORT	PORTD
#define CS_DDR	DDRD
#define CS_MASK	_BV(3)

#define RESET_CAPT_PORT	PORTD
#define RESET_CAPT_DDR	DDRD
#define RESET_CAPT_MASK	_BV(4)

void setup_hw()
{
	/* Setup UART */
	UCSR0A = 1 << U2X0;
	uint16_t br = (F_CPU / 115200 / 8) - 1;
	UBRR0H = br >> 8;
	UBRR0L = br;

	printf_init();
	printf("Initializing...");

	/* Initialize I2C */
	i2c_init();

	/* Configure output pins */
	SCLK_DDR |= SCLK_MASK;
	SS_DDR |= SS_MASK;
	MR_DDR |= MR_MASK;
	CS_DDR |= CS_MASK;
	RESET_CAPT_DDR |= RESET_CAPT_MASK;

	/* Setup SPI */
	SS_PORT |= SS_MASK; // SS Always high. Low would cause us to go into slave-mode.
	SPCR |= _BV(MSTR);
	SPCR |= _BV(SPE);

	// CS high
	CS_PORT |= CS_MASK;

	// Reset the CPLD
	MR_PORT &= ~MR_MASK;
	MR_PORT |= MR_MASK;

	/* Do this once to store the PLL values in the CDCE925 EEPROM */
	//cdce925_init();
	//burn();

	/* Setup interrupts */
	MCUCR = 0;  // Trigger INT0 on low level
	EIMSK = 1;
	sei();		// Enable

	printf("OK\r\n");
}

#define BUFSIZE 32
static volatile uint32_t capture_buffer[BUFSIZE];
static volatile int8_t head = 0;
static volatile int8_t tail = -1;
static volatile int8_t used = 0;

ISR(INT0_vect)
{
	if(used == BUFSIZE)
		return;

	/* Read the capture register */
	CS_PORT &= ~CS_MASK;
	uint8_t a = spi_inout(0x00);
	uint8_t b = spi_inout(0x00);
	uint8_t c = spi_inout(0x00);
	uint8_t d = spi_inout(0x00);
	CS_PORT |= CS_MASK;

	/* Re-enable capture */
	RESET_CAPT_PORT |= RESET_CAPT_MASK;
	RESET_CAPT_PORT &= ~RESET_CAPT_MASK;

	if(tail == BUFSIZE - 1)
		tail = -1;

	capture_buffer[++tail] = (uint32_t) a << 24 | (uint32_t) b << 16 | (uint32_t) c << 8 | (uint32_t) d;
	used++;

	UCSR0B = 1 << 0 | 1 << 1 | 1 << 3 | 1 << 4;
}

void main()
{
	setup_hw();

	while (1)
	{
		while (used)
		{
			uint32_t data = capture_buffer[head++];
			if(head == BUFSIZE)
				head = 0;
			used--;

			printf("capture %ld\r\n", data);
		}
	}
}

