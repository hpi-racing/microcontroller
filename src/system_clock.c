#include "system_clock.h"
//#include <rtc_driver.h>

uint16_t real_time_clock_adjustment;

void system_clock_init (void)
{
	// Nach einem Reset ist der interne 2 MHz Oszillator aktiv
	//
	//	- Internen 32 MHz Oszillator aktivieren -> Basis des Systemtaktes
	//	- Internen 32 kHz Oszillator aktivieren -> Kallibrierung des 32 MHz Oszillators
	OSC_CTRL |= OSC_RC32MEN_bm | OSC_RC32KEN_bm;
	// Warten bis der 32 kHz Oszillator stabil arbeitet
	while ((OSC_STATUS & OSC_RC32KRDY_bm) == 0);

	// 32 MHz Oszillator mit dem 32 kHz Oszillator kallibrieren
	OSC_DFLLCTRL = 0x00;
	DFLLRC32M_CTRL = DFLL_ENABLE_bm;

	// Warten bis der 32 MHz Oszillator stabil arbeitet
	while ((OSC_STATUS & OSC_RC32MRDY_bm) == 0);


	// 32 MHz Oszillator als Quelle des Systemtaktes auswählen
	CCP = CCP_IOREG_gc;
	CLK_CTRL = CLK_SCLKSEL_RC32M_gc;
	// Vorteiler A, B und C jeweils auf den Wert Eins einstellen
	//	-> Systemtakt:   32 MHz
	CCP = CCP_IOREG_gc;
	CLK_PSCTRL = CLK_PSADIV_1_gc | CLK_PSBCDIV_1_1_gc;
	// Einstellungen zum Systemtakt schützen
	CCP = CCP_IOREG_gc;
	CLK_LOCK = CLK_LOCK_bm;


	// Alle nicht benutzten Oszillatoren abschalten
	OSC_CTRL &= (OSC_RC32MEN_bm | OSC_RC32KEN_bm);
	
	//RTC einschalten
	
	//CLKSYS_RTC_ClockSource_Enable(CLK_RTCSRC_ULP_gc);
	
	RTC.CTRL = 0b00000001;
	CLK.RTCCTRL = 0b00000001;
	RTC.INTCTRL |= RTC_OVFINTLVL0_bm;
	
	while ((RTC.STATUS & RTC_SYNCBUSY_bm) == RTC_SYNCBUSY_bm)
		;

	real_time_clock_adjustment = 0;
}

ISR(RTC_OVF_vect)
{
}