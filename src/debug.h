#ifndef DEBUG_H_
#define DEBUG_H_

#include <asf.h>


#ifdef __DEBUG_BOARD__

// Allows debug output with the PIN1_bm, PIN2_bm and PIN3_bm bitmasks.
// Keeps the values of pins 0 through 4 on port B.
#define DBG_OUT(Mask, Value) \
		PORTB_OUT = (PORTB_OUT & (~(((Mask) & ~PIN0_bm) << 4))) \
					| (((Value) & ((Mask) & ~PIN0_bm)) << 4);

// Puts the given byte on debug pins 3 and 2.
// The high order nibble will be on pin 3 in MSB order.
// The low order nibble will be on pin 2 in MSB order.
#define DBG_BYTE(b) \
		PORTB_OUT |= 0xC0; \
		PORTB_OUT &= 0x3F; \
		PORTB_OUT |= (((b) & 0x80) << 0) | (((b) & 0x08) << 3); \
		PORTB_OUT &= 0x3F; \
		PORTB_OUT |= (((b) & 0x40) << 1) | (((b) & 0x04) << 4); \
		PORTB_OUT &= 0x3F; \
		PORTB_OUT |= (((b) & 0x20) << 2) | (((b) & 0x02) << 5); \
		PORTB_OUT &= 0x3F; \
		PORTB_OUT |= (((b) & 0x10) << 3) | (((b) & 0x01) << 6); \
		PORTB_OUT &= 0x3F;
#else

#define DBG_OUT(Mask, Value)
#define DBG_BYTE(b) 

#endif // __DEBUG_BOARD__


// öffentliche Funktionen
void debug_init (void);

#endif /* DEBUG_H_ */