#include "debug.h"
#include "lane_sensor_isr.h"
#include "system_clock.h"

lane_sensor_packet_t lane_sensor_packet;

uint8_t wait_for_upcoming_short = 0;
uint8_t current_bit;
uint8_t bit_count;
uint16_t bit_sequence;

uint16_t last_timer_value;
uint8_t timer_increment;

void adjust_real_time_clock(void);

/**
 * Initialisiert den Eingang für den Schienensensor (Port R Pin 0)
 */
void lane_sensor_init_isr(void)
{
	// Interrupts für Flanken des Schienensignals
	PORTR.INT0MASK = PIN0_bm;				// Interrupt 0 benutzen
	PORTR.INTCTRL |= PORT_INT0LVL_HI_gc;	// Interrupt 0 für Signal der Schiene auf mittleres Level festlegen
	
	// Interrupts für das Ende des Datenpakets auf dem Schienensignal
	TCC0.CTRLA = TC_CLKSEL_DIV64_gc;		// Vorteiler 64 einstellen (Systemtakt: 32 MHz -> Timerfrequenz: 500 kHz)
	TCC0.CCA = 70;							// Interrupt löst bei diesem Counterwert aus
	TCC0.INTCTRLB = TC_CCAINTLVL_OFF_gc;	// Interript erstmal dekativiert lassen
	
	lane_sensor_packet.read_access = READ_ACCESS_NOT_READY;

	last_timer_value = RTC_VALUE;
	timer_increment = 8;
}


/**
 * Interrupt-Service-Routine für Flanken 
 */
ISR (PORTR_INT0_vect)
{
	adjust_real_time_clock();

	uint16_t timer_value = TCC0.CNT;
	TCC0.CNT = 1;	// Counterwert für den Interrupt zum Erkennen des Datenpaketendes zurücksetzen

	// Start des Datenpakets
	if (TCC0.INTCTRLB == TC_CCAINTLVL_OFF_gc)
	{
		wait_for_upcoming_short = false;
		bit_sequence = 0;
		bit_count = 0;
		current_bit = 1;
		TCC0.INTCTRLB = TC_CCAINTLVL_MED_gc; // Timerinterrupt zum Erkennen des Paketendes aktivieren
	}
	// Datenpaket läuft bereits
	else
	{
		// Dekodierung des Manchester-Codes:
		//   einmal lang = Wert des Bits wird umgekehrt
		//   zweimal kurz = gleiche Bit wie das davor
		
		if (timer_value > 35) // einmal lang
		{
			current_bit = (current_bit) ? 0 : 1;
		}
		else if (wait_for_upcoming_short) // zweites kurz
		{
			wait_for_upcoming_short = false;
		}
		else // das erste kurz
		{
			wait_for_upcoming_short = true; // danach auf ein zweites warten
			return;
		}
		
		bit_sequence <<= 1;
		bit_sequence |= current_bit;
		bit_count++;
	}
}


/**
 * Interrupt-Service-Routine für Ende des Datenpakets auf dem Schienensignal
 */
ISR (TCC0_CCA_vect)
{
	TCC0.INTCTRLB = TC_CCAINTLVL_OFF_gc; // Timerinterrupt deaktivieren
	
	if (lane_sensor_packet.read_access == READ_ACCESS_IN_PROGRESS)
		return;
	
	lane_sensor_packet.bit_count = bit_count;
	lane_sensor_packet.bit_sequence = bit_sequence;
	lane_sensor_packet.read_access = READ_ACCESS_READY;
}

inline void adjust_real_time_clock()
{
	uint8_t duration = RTC_VALUE - last_timer_value;

	if (duration < 4)
		return;

	last_timer_value += timer_increment;
	real_time_clock_adjustment += timer_increment - duration;
	timer_increment = (timer_increment == 8) ? 7 : 8;

}