#include "lane_sensor.h"
#include "usb.h"
#include "debug.h"

unsigned char waitForUpcomingShort = 0;
unsigned char idx;
unsigned char currentBit;
int sequence;

/**
 * Initialisiert den Eingang für den Schienensensor (Port R Pin 0)
 */
void lane_sensor_init (void)
{
	// Interrupts für Flanken des Schienensignals
	PORTR_INT0MASK = PIN0_bm;				// Interrupt 0 benutzen
	PORTR_INTCTRL |= PORT_INT0LVL_MED_gc;	// Interrupt 0 für Signal der Schiene auf mittleres Level festlegen
	
	// Interrupts für das Ende des Datenpakets auf dem Schienensignal
	TCC0_CTRLA = TC_CLKSEL_DIV64_gc;		// Vorteiler 64 einstellen (Systemtakt: 32 MHz -> Timerfrequenz: 500 kHz)
	TCC0_CCA = 70;							// Interrupt löst bei diesem Counterwert aus
	TCC0_INTCTRLB = TC_CCAINTLVL_OFF_gc;	// Interript erstmal dekativiert lassen
}


/**
 * Interrupt-Service-Routine für Flanken 
 */
ISR (PORTR_INT0_vect)
{	
	DBG_OUT(PIN3_bm,0xff);
	
	int sensorCount = TCC0_CNT;
	TCC0_CNT = 1;	// Counterwert für den Interrupt zum Erkennen des Datenpaketendes zurücksetzen
		
	// Start der Datenpakets
	if (TCC0_INTCTRLB==TC_CCAINTLVL_OFF_gc)
	{
		sequence = 0;
		idx = 0;
		currentBit = 1;
		TCC0_INTCTRLB = TC_CCAINTLVL_MED_gc; // Timerinterrupt zum Erkennen des Paketendes aktivieren
	}
	// Datenpaket läuft bereits
	else
	{
		// Dekodierung des Manchester-Codes:
		//   einmal lang = Wert des Bits wird umgekehrt
		//   zweimal kurz = gleiche Bit wie das davor
		
		if (sensorCount>35) // einmal lang
		{
			currentBit = (currentBit) ? 0 : 1;
			sequence |= currentBit << idx++;
		}
		else if (waitForUpcomingShort) // zweites kurz
		{
			waitForUpcomingShort = false;
		}
		else // das erste kurz
		{
			waitForUpcomingShort = true; // danach auf ein zweites warten
			sequence |= currentBit << idx++;
		}
	}
	
	DBG_OUT(PIN3_bm,0x00);
}


/**
 * Interrupt-Service-Routine für Ende des Datenpakets auf dem Schienensignal
 */
ISR (TCC0_CCA_vect)
{
	TCC0_INTCTRLB = TC_CCAINTLVL_OFF_gc; // Timerinterrupt deaktivieren
	DBG_OUT(PIN3_bm,0xff);
			
	usb_send(idx | 0b10000000);
	usb_send(sequence & 0b01111111);
	usb_send((sequence >> 7) & 0b01111111);

	DBG_OUT(PIN3_bm,0x00);
}
