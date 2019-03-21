#ifndef PINDEFS_H
#define PINDEFS_H

#define SCLK_PORT   PORTB
#define SCLK_DDR    DDRB
#define SCLK_MASK   _BV(5)

#define SS_PORT PORTB
#define SS_DDR  DDRB
#define SS_MASK _BV(2)

#define MR_PORT PORTC
#define MR_DDR  DDRC
#define MR_MASK _BV(3)

#define CS_PORT PORTD
#define CS_DDR  DDRD
#define CS_MASK _BV(3)

#define RESET_CAPT_PORT PORTD
#define RESET_CAPT_DDR  DDRD
#define RESET_CAPT_MASK _BV(4)

#endif // PINDEFS_H
