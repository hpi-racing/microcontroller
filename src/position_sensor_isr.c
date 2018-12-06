#include "debug.h"
#include "position_sensor_isr.h"

#include <string.h>

position_sensor_state_t position_sensor_states[POSITION_SENSOR_COUNT];
uint16_t last_rising_times[POSITION_SENSOR_COUNT];
uint8_t position_sensor_port_changed;

uint8_t port_history[POSITION_PORT_COUNT];


void enable_port(PORT_t *port);
inline void handle_pin_changes(uint8_t port_index, uint8_t port_value);

void position_sensor_init_isr()
{
	memset(position_sensor_states, 0, sizeof(position_sensor_states));
	memset(last_rising_times, 0, sizeof(last_rising_times));
	memset(port_history, 0, sizeof(port_history));
	
	position_sensor_port_changed = 0;
	TCC1.CTRLA = TC_CLKSEL_DIV64_gc;
	
	enable_port(&PORTC);
	enable_port(&PORTD);
	enable_port(&PORTE);
	enable_port(&PORTF);
}

void enable_port(PORT_t *port)
{
	// Interrupts für Flanken des Positionssensors
	port->INT0MASK |= 0xFF;					//Enable all Pins
	port->INTCTRL |= PORT_INT0LVL_MED_gc;
	
	// Wir brauchen beide Flanken um festzustellen, welche Pins
	// sich geändert haben.
	port->PIN0CTRL |= PORT_ISC_BOTHEDGES_gc;
	port->PIN1CTRL |= PORT_ISC_BOTHEDGES_gc;
	port->PIN2CTRL |= PORT_ISC_BOTHEDGES_gc;
	port->PIN3CTRL |= PORT_ISC_BOTHEDGES_gc;
	port->PIN4CTRL |= PORT_ISC_BOTHEDGES_gc;
	port->PIN5CTRL |= PORT_ISC_BOTHEDGES_gc;
	port->PIN6CTRL |= PORT_ISC_BOTHEDGES_gc;
	port->PIN7CTRL |= PORT_ISC_BOTHEDGES_gc;
}


ISR (PORTC_INT0_vect)
{
	handle_pin_changes(1, PORTC.IN);
}

ISR (PORTD_INT0_vect)
{
	handle_pin_changes(2, PORTD.IN);
}

ISR (PORTE_INT0_vect)
{
	handle_pin_changes(3, PORTE.IN);
}

ISR (PORTF_INT0_vect)
{
	handle_pin_changes(4, PORTF.IN);
}

void handle_pin_changes(uint8_t port_index, uint8_t port_value)
{
	// Der Port-Wert sollte nur einmal aus dem PORT_t.IN Register
	// gelesen werden, da sich der Registwert während dieser Instruktionen
	// hier verändern kann.
	// Das kann dann dazu führen, dass wir Pins, die sich geändert haben
	// nicht behandeln und damit längere Periodendauern berechnen.
	uint8_t high_pins = (port_value ^ port_history[port_index]) & port_value; //pins which switched to One
	uint8_t low_pins = (port_value ^ port_history[port_index]) & (~port_value);
	port_history[port_index] = port_value;
	uint16_t current_time = TCC1.CNT;
	
	while (high_pins)
	{
		uint8_t pin = ctz8(high_pins); // get next pin
		uint8_t sensor_id = pin + 8*port_index;
		last_rising_times[sensor_id] = current_time;
		
		high_pins &= ~(_BV(pin)); // clear pin
	}
	
	while (low_pins)
	{
		uint8_t pin = ctz8(low_pins); // get next pin
		uint8_t sensor_id = pin + 8*port_index;
		position_sensor_state_t *sensor = position_sensor_states + sensor_id;
		
		low_pins &= ~(_BV(pin));
		
		if (sensor->read_access == READ_ACCESS_IN_PROGRESS)
			continue;
		
		uint16_t high_period = current_time - last_rising_times[sensor_id];
		
		if (high_period <= 8)
			continue;
		
		for (int i = POSITION_SENSOR_TIMESTAMP_COUNT - 1; i > 0; i--)
		{
			sensor->timestamps[i] = sensor->timestamps[i-1];
		}
		sensor->timestamps[0] = current_time;
		sensor->read_access = READ_ACCESS_READY;
		
		position_sensor_port_changed |= _BV(port_index);
	}
}