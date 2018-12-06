#include "lane_sensor.h"
#include "lane_sensor_isr.h"
#include "read_access.h"
#include "usb.h"
#include "debug.h"


#include <asf.h>
#include <string.h>

inline uint16_t reverse12(uint16_t v);
inline uint8_t reverse8(uint8_t v);

void lane_sensor_init()
{
	lane_sensor_init_isr();
}

void process_lane_sensor_queue()
{
	if (lane_sensor_packet.read_access != READ_ACCESS_READY)
		return;
	
	lane_sensor_packet.read_access = READ_ACCESS_IN_PROGRESS;
	uint8_t count = lane_sensor_packet.bit_count;
	uint16_t sequence = lane_sensor_packet.bit_sequence;
	lane_sensor_packet.read_access = READ_ACCESS_NOT_READY;
	
	if (count == 7)
	{
		// Umdrehen, damit das Aktivbit für Controller 0 auch das Bit 0 ist.
		uint8_t payload = reverse8(sequence) >> 1;
		usb_send_packet(PACKET_TYPE_CONTROLLER_ACTIVITY, &payload, sizeof(payload));
	}
	else if (count == 9)
	{
		uint8_t payload[2];
		packet_type_t type = ((sequence >> 6) == 7) ? PACKET_TYPE_RACE_STATUS : PACKET_TYPE_CONTROLLER_STATE;
		payload[0] = sequence >> 1;
		payload[1] = sequence << 7;

		usb_send_packet(type, payload, sizeof(payload));
	}
	else if (count == 12)
	{
		// Die Control Unit schickt Programmierdatenworte in LSB
		// Reihenfolge. Für Einheitlichkeit wird die Bitsequenz deshalb
		// umgekehrt bevor sie per USB verschickt wird.
		sequence = reverse12(sequence);

		uint8_t payload[2];
		payload[0] = sequence >> 4;
		payload[1] = sequence << 4;

		usb_send_packet(PACKET_TYPE_TRACK_CONTROL, payload, sizeof(payload));
	}
}

uint16_t reverse12(uint16_t v)
{
	// swap even and odd bits
	v = ((v >> 1) & 0x0555) | ((v & 0x0555) << 1);
	// swap consecutive pairs
	v = ((v >> 2) & 0x0333) | ((v & 0x0333) << 2);
	// swap first and last nibble
	v = ((v >> 8) & 0x000F) | (v & 0x00F0) | ((v & 0x000F) << 8);
	
	return v;
}

uint8_t reverse8(uint8_t v)
{
	// swap even and odd bits
	v = ((v >> 1) & 0x55) | ((v & 0x55) << 1);
	// swap consecutive pairs
	v = ((v >> 2) & 0x33) | ((v & 0x33) << 2);
	// swap first and last nibble
	v = ((v >> 4) & 0x0F) | ((v & 0x0F) << 4);
	
	return v;
}