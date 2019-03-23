#include <stdio.h>
#include <avr/interrupt.h>
#include "ttys.h"

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
