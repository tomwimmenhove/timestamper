#ifndef SPI2_H
#define SPI2_H

/* Stolen from the Arduino SPI.h header */
static inline uint8_t spi_inout(uint8_t data)
{
	SPDR = data;
	/*
	 * The following NOP introduces a small delay that can prevent the wait
	 * loop form iterating when running at the maximum speed. This gives
	 * about 10% more speed, even if it seems counter-intuitive. At lower
	 * speeds it is unnoticed.
	 */
	__asm__ volatile("nop");
	while (!(SPSR & _BV(SPIF))) ; // wait
	return SPDR;
}


#endif // SPI2_H

