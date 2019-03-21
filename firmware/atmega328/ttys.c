#include <stdio.h>
#include <avr/interrupt.h>

void uart_putchar(char c)
{
	loop_until_bit_is_set(UCSR0A, UDRE0); /* Wait until data register empty. */
	UDR0 = c;
}

static int uart_putchar_printf(char c, FILE *stream)
{
	uart_putchar(c);

	return 0;
}

static FILE ttys = FDEV_SETUP_STREAM(uart_putchar_printf, NULL, _FDEV_SETUP_WRITE);

void printf_init()
{
	stdout = &ttys;
}
