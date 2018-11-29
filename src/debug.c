#include "debug.h"

void debug_init (void)
{
	// JTAG-Interface abschalten (benutzt die oberen 4 Bits des Ports B)
	CCP = CCP_IOREG_gc;
	MCU_MCUCR = MCU_JTAGD_bm;
	
#ifdef __DEBUG_BOARD__
	// Die oberen 4 Pins des Ports B als Ausg�nge konfigurieren
	PORTB_DIR |= 0xF0;
#else
	// Die oberen 4 Pins des Ports B als Eing�nge konfigurieren
	PORTB_DIR &= ~0xF0;
#endif
}