#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/twi.h>
#include <string.h>
#include <util/delay.h>

#include "ttys.h"
#include "i2c_master.h"
#include "spi.h"
#include "circ_buf.h"
#include "pindefs.h"

void setup_hw()
{
	/* Setup UART */
#define BAUD 57600
//#define BAUD 38400
#include <util/setbaud.h>
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);
#if USE_2X
	UCSR0A |= (1 << U2X0);
#else
	UCSR0A &= ~(1 << U2X0);
#endif

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

	/* CS high */
	CS_PORT |= CS_MASK;

	/*Reset the CPLD */
	MR_PORT &= ~MR_MASK;
	MR_PORT |= MR_MASK;

	/* Wait at least a second before we start capturing, in order to make sure 
	 * we've captured a PPS pulse */
	for (int i = 0; i < 100; i++)
		_delay_ms(10);

	/* Re-set capture in case a capture event happened while we were waiting. */
	RESET_CAPT_PORT |= RESET_CAPT_MASK;
	RESET_CAPT_PORT &= ~RESET_CAPT_MASK;

	/* Do this once to store the PLL values in the CDCE925 EEPROM */
	//cdce925_init();
	//cdce925_burn();

	/* Setup interrupts */
	MCUCR = 0;  // Trigger INT0 on low level
	EIMSK = 1;
	sei();		// Enable

	printf("OK\r\n");
}

#define BUFSIZE 32	// Power of two
static volatile uint32_t capture_buffer[BUFSIZE];
static volatile int8_t tail = 0;
static volatile int8_t head = 0;

ISR(INT0_vect)
{
	if(!CIRC_SPACE(head, tail, BUFSIZE))
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

	capture_buffer[head++] = (uint32_t) a << 24 | (uint32_t) b << 16 | (uint32_t) c << 8 | (uint32_t) d;
	head %= BUFSIZE;

	UCSR0B = 1 << 0 | 1 << 1 | 1 << 3 | 1 << 4;
}

void main()
{
	setup_hw();

	while (1)
	{
		while (CIRC_CNT(head, tail, BUFSIZE))
		{
			uint32_t data = capture_buffer[tail++];
			tail %= BUFSIZE;

			printf("%ld\r\n", data);
		}
	}
}

