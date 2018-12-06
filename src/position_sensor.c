#include <asf.h>
#include <string.h>

#include "position_sensor.h"
#include "position_sensor_isr.h"
#include "usb.h"
#include "debug.h"


#define MAX_CONTROLLER_ID 7
#define MAX_PERIOD_OFFSET 15

#define PERIOD_COUNT (POSITION_SENSOR_TIMESTAMP_COUNT)-1

uint8_t check_port;

void process_sensor(position_sensor_state_t *sensor, uint8_t sensor_id);
inline uint8_t period_to_controller_id(uint16_t period);

void position_sensor_init(void)
{
	position_sensor_init_isr();
	
	check_port = 0;
}

void process_position_sensor_queue()
{
	check_port = (check_port == POSITION_PORT_COUNT-1) ? 0 : check_port + 1;
	
	if (!(position_sensor_port_changed & _BV(check_port)))
		return;
	
	position_sensor_port_changed &= ~_BV(check_port);
	
	for (uint8_t sensor_id=8*check_port; sensor_id<8*(check_port+1); sensor_id++)
	{
		process_sensor(position_sensor_states + sensor_id, sensor_id);
	}
}

void process_sensor(position_sensor_state_t *sensor, uint8_t sensor_id)
{
	if (sensor->read_access != READ_ACCESS_READY)
		return;
	
	sensor->read_access = READ_ACCESS_IN_PROGRESS;
	position_sensor_state_t copy = *sensor;
	sensor->read_access = READ_ACCESS_NOT_READY;
	
	uint16_t period = copy.timestamps[0] - copy.timestamps[1];
	uint8_t controller_id = period_to_controller_id(period);
	
	if (controller_id > MAX_CONTROLLER_ID)
		return;
	
	for (int i=1; i<PERIOD_COUNT; i++)
	{
		period = copy.timestamps[i] - copy.timestamps[i+1];
		uint8_t tmp_controller_id = period_to_controller_id(period);
		
		if (tmp_controller_id != controller_id)
			return;
	}
	
	uint8_t data[2];
	data[0] = controller_id << 5;
	data[1] = sensor_id;
	
	usb_send_packet(PACKET_TYPE_POSITION, data, 2);
}

uint8_t period_to_controller_id(uint16_t period)
{
	uint8_t result = ((period + MAX_PERIOD_OFFSET) >> 5) - 1;
	
	if (result > MAX_CONTROLLER_ID)
		return 0xFF;
	
	uint8_t ideal_period = (result + 1) << 5;
	int8_t period_offset = period - ideal_period;
	int8_t tolerable_offset = (result + 1) << 1;
	
	if (period_offset > tolerable_offset || period_offset < -tolerable_offset)
		return 0xFF;
	
	return result;
}
