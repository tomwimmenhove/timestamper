#ifndef PRINTF_H
#define PRINTF_H

static inline void uart_putchar(char c)
{
	loop_until_bit_is_set(UCSR0A, UDRE0); /* Wait until data register empty. */
	UDR0 = c;
}

void printf_init();


#endif // PRINTF_H

