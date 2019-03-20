#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/twi.h>
#include <string.h>

#include "printf.h"
#include "i2c_master.h"

void setup_hw()
{
	UCSR0A = 1 << U2X0;
	uint16_t br = (16000000 / 115200 / 8) - 1;
	UBRR0H = br >> 8;
	UBRR0L = br;

	printf_init();

	printf("Initializing...");
	i2c_init();

	// Outputs
	DDRB |= 1 << 5; // SCLK
	DDRB |= 1 << 2; // /SS
	DDRC |= 1 << 3; // Master reset
	DDRD |= 1 << 3; // CE
	DDRD |= 1 << 4; // Reset capture

	PORTB |= 1 << 2; // /SS always high

	// SPI
	SPCR |= _BV(MSTR);
	SPCR |= _BV(SPE);

	// CS high
	PORTD |= 1 << 3;

	// Reset the CPLD
	PORTC &= ~(1 << 3);
	PORTC |= 1 << 3;

	MCUCR = 0;  // Trigger INT0 on low level
	EIMSK = 1;

	//cdce925_init();

	//burn();

	sei(); // Enable interrupts
	printf("OK\r\n");
}


void i2c_read_test()
{
	uint8_t id = read_reg(CDCE925_ADDR, 0x80 | 46);
	printf("id: 0x%02x\n", id);
}

#define BUFSIZE 64
static volatile uint32_t capture_buffer[BUFSIZE];
static volatile int8_t head = 0;
static volatile int8_t tail = -1;
static volatile int8_t used = 0;

/* Stolen from the Arduino SPI.h header */
uint8_t spi_inout(uint8_t data)
{
	SPDR = data;
	/*
	 * The following NOP introduces a small delay that can prevent the wait
	 * loop form iterating when running at the maximum speed. This gives
	 * about 10% more speed, even if it seems counter-intuitive. At lower
	 * speeds it is unnoticed.
	 */
	//  asm volatile("nop");
	while (!(SPSR & _BV(SPIF))) ; // wait
	return SPDR;

}

//void read_data()
ISR(INT0_vect)
{
	if(used == BUFSIZE)
		return;

	/* Read the capture register */
	PORTD &= ~(1 << 3);
	uint8_t a = spi_inout(0x00);
	uint8_t b = spi_inout(0x00);
	uint8_t c = spi_inout(0x00);
	uint8_t d = spi_inout(0x00);
	PORTD |= 1 << 3;

	/* Re-enable capture */
	PORTD |= 1 << 4;
	PORTD &= ~(1 << 4);

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

